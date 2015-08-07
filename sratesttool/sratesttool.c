/*
 * Copyright (c) 2015 by Farsight Security, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <sysexits.h>

#include <axa/client.h>
#include <axa/axa_endian.h>


/* connect to server
 *
 * returns true on success, false on failure
 */
static bool srvr_connect(void);

/* send an AXA command to the server and wait for the response
 *
 * returns true on success, false on failure
 */
static bool srvr_cmd(axa_tag_t,             /* AXA tag or AXA_TAG_NONE */
                        axa_p_op_t,         /* AXA opcode (original) */
                        const void *,       /* body of message to send */
                        size_t,             /* length of message */
                        axa_p_op_t);        /* expected response opcode */

/* wait for a response from the server
 *
 * returns true on success, false on failure
 */
static bool srvr_wait_resp(axa_p_op_t,      /* expected response opcode */
                        axa_p_op_t);        /* original opcode*/

/* process a message from the server */
static void srvr_process(void);

/* disconnect from server and print a message */
static void srvr_disconnect(const char *,   /* message to print */
                        ...) AXA_PF(1,2);   /* optional varargs */

/* stop everything and shutdown */
static void stop(int);                      /* exitcode */

/* signal handler */
static void sigterm(int);                   /* signal number of caught sig */

/* print usage */
static void usage(const char *);            /* program name */

static axa_client_t client;
static const char *server_str;
static const char *watch_str;
static const char *channel_str;
static int terminated;
static uint64_t hits;


int
main(int argc, char **argv)
{
    int opt;
    axa_emsg_t emsg;

    while ((opt = getopt(argc, argv, "c:hs:w:")) >= 0)
    {
        switch (opt)
        {
            case 'c':
                channel_str = optarg;
                break;
            case 'h':
                usage(argv[0]);
                return (EXIT_SUCCESS);
            case 's':
                server_str = optarg;
                break;
            case 'w':
                watch_str = optarg;
                break;
            default:
                usage(argv[0]);
                return (EX_USAGE);
        }
    }
    if (server_str == NULL || channel_str == NULL || watch_str == NULL)
    {
        fprintf(stderr, "-s, -c, -w are all required\n");
        return (EX_USAGE);
    }

    /* set the global program name (for logging) */
    axa_set_me(argv[0]);
    /* set the tracing stream to emit to stderr only */
    AXA_ASSERT(axa_parse_log_opt(&emsg, "trace,off,stderr"));
    /* set the error stream to emit to stderr only */
    AXA_ASSERT(axa_parse_log_opt(&emsg, "error,off,stderr"));
    /* set the accounting stream to emit to stderr only */
    AXA_ASSERT(axa_parse_log_opt(&emsg, "acct,off,stderr"));

    /* initialize the AXA syslog interface */
    axa_syslog_init();

    /* ensure all stdio FDs are open and ready for business */
    axa_clean_stdio();

    /* initialize the client context (including AXA IO engine) */
    axa_client_init(&client);

    /* catch and handle these signals for graceful exit */
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, sigterm);
    signal(SIGTERM, sigterm);
    signal(SIGINT, sigterm);

    axa_trace_msg("Farsight Security SRA Test Tool\n");

    /* try (once) to connect to the SRA server and bail on any error */
    if (!srvr_connect())
    {
        exit(EXIT_FAILURE);
    }

    /* event loop where we continuously wait/send data/wait */
    for (;;)
    {
        if (terminated != 0)
        {
            stop(terminated);
        }
        /* wait up to 10ms for data from the server */
        switch (axa_io_wait(&emsg,
                    &client.io,         /* address of AXA IO engine */
                    10,                 /* wait up to this many ms */
                    false,              /* wake up to send a keepalive */
                    false))             /* pay attention to tunnel msgs */
        {
            /* AXA IO error */
            case AXA_IO_ERR:
                srvr_disconnect("%s", emsg.c);
                break;
            /* incomplete response, poll() and try again */
            case AXA_IO_BUSY:
                break;
            /* operation has finished, time to process the result */
            case AXA_IO_OK:
                srvr_process();
                break;
            default:
                AXA_FAIL("impossible axa_io_wait() result");
        }
    }
}

static bool
srvr_connect(void)
{
    axa_p_watch_t watch;
    size_t watch_len;
    axa_p_channel_t channel;
    axa_emsg_t emsg;
    bool res;
    axa_tag_t cur_tag;

    axa_trace_msg("connecting to %s...", server_str);
    switch (axa_client_open(&emsg,      /* if func fails, will contain error */
                            &client,    /* client context */
                            server_str, /* server address string */
                            false,      /* true to enable RAD mode */
                            false,      /* true when SSH tunnel debugging */
                            256 * 1024, /* socket buffer size */
                            false))     /* true for non-blocking */
    {
        /* permanent failure, connection has been closed, check emsg */
        case AXA_CONNECT_ERR:
            axa_error_msg("%s", emsg.c);
            return (false);
        /* A temporary failure, connection has been closed, check emsg.
         * In more robust implementations, we would try to reconnect to the
         * server.
         */
        case AXA_CONNECT_TEMP:
            axa_error_msg("%s", emsg.c);
            srvr_disconnect("%s", emsg.c);
            return (false);
        /* connect is complete */
        case AXA_CONNECT_DONE:
            break;
        /* non-blocking connection waiting for TCP SYN/ACK or TLS handshake */
        case AXA_CONNECT_INCOM:
            AXA_FAIL("impossible result from axa_client_open()");
        /* connect is complete (incl xmit of AXA_P_OP_NOP) */
        case AXA_CONNECT_NOP:
            break;
        /* connect is complete (incl xmit of AXA_P_OP_USER) */
        case AXA_CONNECT_USER:
            AXA_FAIL("unexpected response from axa_client_open()");
    }

    /* block any watch hits until we are ready */
    if (!srvr_cmd(AXA_TAG_MIN, AXA_P_OP_PAUSE, NULL, 0, AXA_P_OP_OK))
    {
        return (false);
    }

    cur_tag = 1;
    /* parse the watch string */
    axa_trace_msg("parsing watch: %s...\n", watch_str);
    res = axa_parse_watch(&emsg, &watch, &watch_len, watch_str);
    if (!res)
    {
        if (emsg.c[0] == '\0')
        {
            axa_error_msg("unrecognized \"-w %s\"", watch_str);
        }
        else
        {
            axa_error_msg("\"-w %s\": %s", watch_str, emsg.c);
        }
        return (false);
    }
    else
    {
        axa_trace_msg("parse %s OK\n", watch_str);
    }

    /* set the watch on the server */
    axa_trace_msg("setting watch on server...\n");
    if (!srvr_cmd(cur_tag, AXA_P_OP_WATCH, &watch, watch_len, AXA_P_OP_OK))
    {
        return (false);
    }
    axa_trace_msg("watch set OK\n");

    memset(&channel, 0, sizeof(channel));
    /* parse the channel string */
    axa_trace_msg("parsing channel: %s...\n", channel_str);
    if (!axa_parse_ch(&emsg, &channel.ch, channel_str,
                strlen(channel_str), true, true))
    {
        axa_error_msg("\"-c %s\": %s", channel_str, emsg.c);
        return (false);
    }
    else
    {
        axa_trace_msg("parse %s OK\n", channel_str);
    }

    channel.on = 1;
    axa_trace_msg("enabling channel on server...\n");
    if (!srvr_cmd(AXA_TAG_MIN, AXA_P_OP_CHANNEL, &channel, sizeof(channel),
        AXA_P_OP_OK))
    {
        return (false);
    }

    if (!srvr_cmd(AXA_TAG_MIN, AXA_P_OP_GO, NULL, 0, AXA_P_OP_OK))
    {
        return (false);
    }
    axa_trace_msg("channel enabled OK\n");

    return (true);
}

static bool
srvr_cmd(axa_tag_t tag, axa_p_op_t op, const void *b, size_t b_len,
     axa_p_op_t resp_op)
{
    axa_p_hdr_t hdr;
    axa_emsg_t emsg;
    char pbuf[AXA_P_STRLEN];

    if (!axa_client_send(&emsg, &client, tag, op, &hdr, b, b_len))
    {
        srvr_disconnect("sending %s failed: %s",
                axa_p_to_str(pbuf, sizeof(pbuf), true, &hdr, b),
                emsg.c);
        return (false);
    }

    return (srvr_wait_resp(resp_op, op));
}

static bool
srvr_wait_resp(axa_p_op_t resp_op, axa_p_op_t orig_op)
{
    bool result, done;
    axa_emsg_t emsg;

    result = false;
    done = false;
    do
    {
        if (terminated != 0)
        {
            stop(terminated);
        }

        switch (axa_input(&emsg, &client.io, INT_MAX))
        {
            case AXA_IO_ERR:
                axa_error_msg("%s", emsg.c);
                goto out;
            case AXA_IO_BUSY:
                continue;
            case AXA_IO_OK:
                break;
            default:
                AXA_FAIL("impossible axa_input() result");
        }
        switch ((axa_p_op_t)client.io.recv_hdr.op)
        {
            case AXA_P_OP_NOP:
                break;
            case AXA_P_OP_HELLO:
                if (!axa_client_hello(&emsg, &client, NULL))
                {
                    axa_error_msg("%s", emsg.c);
                }
                else
                {
                    axa_trace_msg("connected OK");
                }
                break;
            case AXA_P_OP_OPT:
                done = true;
                break;
            case AXA_P_OP_OK:
                if (resp_op == client.io.recv_hdr.op &&
                        orig_op == client.io.recv_body->result.orig_op)
                {
                    result = true;
                    done = true;
                }
                break;
            case AXA_P_OP_ERROR:
                axa_error_msg("server returned error");
                done = true;
                break;
            case AXA_P_OP_MISSED:
            case AXA_P_OP_MISSED_RAD:
            case AXA_P_OP_WHIT:
            case AXA_P_OP_AHIT:
            case AXA_P_OP_WLIST:
            case AXA_P_OP_ALIST:
            case AXA_P_OP_CLIST:
                /* in this function, these are all unexpected op codes */
                break;
            case AXA_P_OP_USER:
            case AXA_P_OP_JOIN:
            case AXA_P_OP_PAUSE:
            case AXA_P_OP_GO:
            case AXA_P_OP_WATCH:
            case AXA_P_OP_WGET:
            case AXA_P_OP_ANOM:
            case AXA_P_OP_AGET:
            case AXA_P_OP_STOP:
            case AXA_P_OP_ALL_STOP:
            case AXA_P_OP_CHANNEL:
            case AXA_P_OP_CGET:
            case AXA_P_OP_ACCT:
            case AXA_P_OP_RADU:
            default:
                AXA_FAIL("impossible AXA op of %d from %s",
                     client.io.recv_hdr.op, client.io.label);
        }
        axa_recv_flush(&client.io);
    }
    while (!done);

out:
    if (!result)
    {
        /* disconnect for now if we failed to get the right response */
        axa_client_backoff(&client);
    }
    return (result);
}

static void AXA_PF(1,2)
srvr_disconnect(const char *p, ...)
{
    va_list args;

    va_start(args, p);
    axa_verror_msg(p, args);
    va_end(args);

    axa_client_backoff(&client);
}

static void AXA_NORETURN
stop(int s)
{
    axa_client_close(&client);
    axa_io_cleanup();

    axa_trace_msg("%"PRIu64" total watch hits\n", hits);

    exit(s);
}

static void
srvr_process(void)
{
    int n;
    char buf[BUFSIZ];
    axa_emsg_t emsg;
    struct timespec ts;
    struct tm *tm_info;

    switch (axa_recv_buf(&emsg, &client.io))
    {
        case AXA_IO_OK:
            break;
        case AXA_IO_ERR:
            srvr_disconnect("%s", emsg.c);
            return;
        case AXA_IO_BUSY:
            return;             /* wait for the rest */
        case AXA_IO_KEEPALIVE:  /* NA for this example */
        case AXA_IO_TUNERR:     /* NA for this example */
            break;
    }

    n = 0;
    switch ((axa_p_op_t)client.io.recv_hdr.op)
    {
        case AXA_P_OP_NOP:
            break;
        case AXA_P_OP_ERROR:
            srvr_disconnect(" ");
            return;
        case AXA_P_OP_MISSED:
        case AXA_P_OP_MISSED_RAD:
            break;
        case AXA_P_OP_WHIT:
            switch (client.io.recv_body->whit.hdr.type)
            {
                case AXA_P_WHIT_NMSG:
                    ts.tv_sec = client.io.recv_body->whit.nmsg.hdr.ts.tv_sec;
                    ts.tv_nsec = client.io.recv_body->whit.nmsg.hdr.ts.tv_nsec;
                    break;
                case AXA_P_WHIT_IP:
                    ts.tv_sec =
                        AXA_P2H32(client.io.recv_body->whit.ip.hdr.tv.tv_sec);
                    ts.tv_nsec = 1000 *
                        AXA_P2H32(client.io.recv_body->whit.ip.hdr.tv.tv_usec);
                    break;
            }
            tm_info = localtime((time_t *)&ts.tv_sec);
            n = strftime(buf + n, 26, "%Y-%m-%dT%H:%M:%S", tm_info);
            snprintf(buf + n, BUFSIZ - n, ".%ld", ts.tv_nsec);
            axa_trace_msg("%s watch hit, channel: %3d @ %s\n",
                    client.io.recv_body->whit.hdr.type == AXA_P_WHIT_NMSG ?
                    "NMSG" : "IP",
                    client.io.recv_body->whit.hdr.ch,
                    buf);
            hits++;
            break;
        case AXA_P_OP_AHIT:
        case AXA_P_OP_OK:
        case AXA_P_OP_HELLO:
        case AXA_P_OP_WLIST:
        case AXA_P_OP_ALIST:
        case AXA_P_OP_OPT:
        case AXA_P_OP_CLIST:
            break;
        case AXA_P_OP_USER:
        case AXA_P_OP_JOIN:
        case AXA_P_OP_PAUSE:
        case AXA_P_OP_GO:
        case AXA_P_OP_WATCH:
        case AXA_P_OP_WGET:
        case AXA_P_OP_ANOM:
        case AXA_P_OP_AGET:
        case AXA_P_OP_STOP:
        case AXA_P_OP_ALL_STOP:
        case AXA_P_OP_CHANNEL:
        case AXA_P_OP_CGET:
        case AXA_P_OP_ACCT:
        case AXA_P_OP_RADU:
        default:
            AXA_FAIL("impossible AXA op of %d from %s",
                 client.io.recv_hdr.op, client.io.label);
    }

    axa_recv_flush(&client.io);
}

void
sigterm(int sig)
{
    terminated = sig;

    signal(sig, SIG_DFL);       /* quit early on repeated signals */
}

static void
usage(const char *name)
{
    printf("SRA Test Tool (c) 2015 Farsight Security, Inc.\n");
    printf("\nUsage: %s [options]\n", name);
    printf("[-s tls:user@server,port]\n");
    printf("\t\tuser:      SRA username\n");
    printf("\t\tserver:    SRA server\n");
    printf("\t\tport:      TCP port (typically 1021)\n");
    printf("[-h]");
    printf("\t\tthis prose\n");
    printf("[-c channel]");
    printf("\tenable channel (default: \"255\", SIE Heartbeat)\n");
    printf("[-w watch]");
    printf("\tset watch (default: \"ch=255\", SIE Heartbeat channel)\n");
}

/*
 * Copyright (c) 2017 by Farsight Security, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <inttypes.h>

#include <nmsg.h>

static int get_socket(const char *);
static void signal_handler(int sig __attribute__((unused)));
static void setup_signals(void);

static nmsg_io_t io;

static void
callback(nmsg_message_t msg, void *user __attribute__((__unused__)))
{
	nmsg_res res;
	char *json_blob;

	/* Serialize NMSG as JSON, freeing the malloc'd string if successful. */
	res = nmsg_message_to_json(msg, &json_blob);
	if (res != nmsg_res_success)
	{
		fprintf(stderr, "x");
	}
	else
	{
		fprintf(stderr, "%s\n", json_blob);
		free(json_blob);
	}

	nmsg_message_destroy(&msg);
}

int
main(int argc, char **argv)
{
	nmsg_res res;		/* reusuable NMSG response holder */
	nmsg_input_t input;	/* NMSG input object */
	nmsg_output_t output;	/* NMSG output object */
	uint64_t count;
	int c, sock = -1;

	while ((c = getopt(argc, argv, "l:")) != -1)
	{
		switch (c)
		{
			case 'l':
				sock = get_socket(optarg);
				break;
			default:
				fprintf(stderr,
					"usage: %s "
					"{-l address/port}\n", argv[0]);
				exit(1);
		}
	}
	if (sock == -1)
	{
		fprintf(stderr, "-l must be specified\n");
		exit(1);
	}
	printf("nmsg JSON emitter / seqsrc example program\n");

	/* Iniitialize nmsg. */
	res = nmsg_init();
	if (res != nmsg_res_success)
	{
		fprintf(stderr, "unable to initialize libnmsg\n");
		return (EXIT_FAILURE);
	}
	printf("nmsg initialized\n");

	/* Iniitialize nmsg IO engine. */
	io = nmsg_io_init();
	if (io == NULL)
	{
		fprintf(stderr, "nmsg_io_init() failed\n");
		return (EXIT_FAILURE);
	}
	printf("nmsg io engine initialized\n");

	/* Initialize a new nmsg stream input. */
	input = nmsg_input_open_sock(sock);
	if (input == NULL)
	{
		fprintf(stderr, "nmsg_input_open_file() failed\n");
		return (EXIT_FAILURE);
	}
	printf("nmsg socket input initialized\n");

	/* Instruct io engine to enable container loss tracking. */
	res = nmsg_input_set_verify_seqsrc(input, true);
	if (res != nmsg_res_success)
	{
		fprintf(stderr, "nmsg_input_set_verify_seqsrc(): %s\n",
			nmsg_res_lookup(res));
		return (EXIT_FAILURE);
	}
	printf("verify seqsrc enabled\n");

	/* Add an nmsg input to an nmsg_io_t object. */
	res = nmsg_io_add_input(io, input, NULL);
	if (res != nmsg_res_success)
	{
		fprintf(stderr, "nmsg_io_add_input(): %s\n",
			nmsg_res_lookup(res));
		return (EXIT_FAILURE);
	}
	printf("socket input added to io engine\n");

	/* Initialize a new nmsg output closure.
	 * This allows a user-provided callback to function as an nmsg
	 * output, for instance to participate in an nmsg_io loop. The
	 * callback is responsible for disposing of each nmsg message. */
	output = nmsg_output_open_callback(callback, NULL);
	if (output == NULL)
	{
		fprintf(stderr, "nmsg_output_open_callback() failed\n");
		return (EXIT_FAILURE);
	}
	printf("callback initialized\n");

	/* Add an nmsg output to an nmsg_io_t object.
	 * When nmsg_io_loop() is called, the input threads will cycle over
	 * and write payloads to the available outputs. */
	res = nmsg_io_add_output(io, output, NULL);
	if (res != nmsg_res_success)
	{
		fprintf(stderr, "nmsg_io_add_output() failed: %s\n",
			nmsg_res_lookup(res));
		return (EXIT_FAILURE);
	}
	printf("callback added to io engine\n");

	/* from nmsgtool */
	setup_signals();

	/* Begin processing the data specified by the configured inputs and
	 * outputs.
	 * One processing thread is created for each input. nmsg_io_loop()
	 * does not return until these threads finish and are destroyed. Only
	 * nmsg_io_breakloop() may be called asynchronously while
	 * nmsg_io_loop() is executing. nmsg_io_loop() invalidates an
	 * nmsg_io_t object. nmsg_io_destroy() should then be called. */
	printf("entering io loop, <ctrl-c> to quit...\n");
	res = nmsg_io_loop(io);
	if (res != nmsg_res_success)
	{
		fprintf(stderr, "nmsg_io_loop() failed: %s\n",
			nmsg_res_lookup(res));
		return (EXIT_FAILURE);
	}

	/* Retrieve the total number of NMSG containers that have been
	 * received since the nmsg_input_t object was created. */
	res = nmsg_input_get_count_container_received(input, &count);
	if (res != nmsg_res_success)
	{
		fprintf(stderr,
			"nmsg_input_get_count_container_received(): %s\n",
			nmsg_res_lookup(res));
		return (EXIT_FAILURE);
	}
	printf("\nreceived %" PRIu64 " containers\n", count);

	/* Retrieve the total number of NMSG containers that have been
	 * dropped since the nmsg_input_t object was created. */
	res = nmsg_input_get_count_container_dropped(input, &count);
	if (res != nmsg_res_success)
	{
		fprintf(stderr,
			"nmsg_input_get_count_container_dropped(): %s\n",
			nmsg_res_lookup(res));
		return (EXIT_FAILURE);
	}
	printf("dropped %" PRIu64 " containers\n", count);

	/* Deallocate the resources associated with an nmsg_io_t object. */
	nmsg_io_destroy(&io);

	return (EXIT_SUCCESS);
}

static int
get_socket(const char *spec)
{
	char *p, *addr;
	struct sockaddr_in6 sa6;
	struct sockaddr_in sa4;
	struct sockaddr *sap;
	int pf, sock, port, len;

	if ((p = strchr(spec, '/')) == NULL)
	{
		p = strchr(spec, ',');
	}
	if (p == NULL)
	{
		fprintf(stderr, "socket spec is addr,port or addr/port\n");
		return (-1);
	}
	port = atoi(p + 1);
	if (port == 0)
	{
		fprintf(stderr, "port number '%s' is not valid\n", p + 1);
		return (-1);
	}
	addr = strndup(spec, p - spec);
	if (inet_pton(AF_INET6, addr, &sa6.sin6_addr) > 0)
	{
		len = sizeof sa6;
		sa6.sin6_family = AF_INET6;
		/* sa6.sin6_len = len; */
		sa6.sin6_port = htons(port);
		pf = PF_INET6;
		sap = (struct sockaddr *) &sa6;
	}
	else if (inet_pton(AF_INET, addr, &sa4.sin_addr) > 0)
	{
		len = sizeof sa4;
		sa4.sin_family = AF_INET;
		/* sa4.sin_len = len; */
		sa4.sin_port = htons(port);
		pf = PF_INET;
		sap = (struct sockaddr *) &sa4;
	}
	else
	{
		fprintf(stderr, "address '%s' is not valid\n", addr);
		free(addr);
		return (-1);
	}
	free(addr);
	sock = socket(pf, SOCK_DGRAM, 0);
	if (sock == -1)
	{
		perror("socket");
		return (-1);
	}
	if (bind(sock, sap, len) == -1)
	{
		perror("bind");
		return (-1);
	}
	return (sock);
}

static void
signal_handler(int sig __attribute__((unused)))
{
	nmsg_io_breakloop(io);
}

static void
setup_signals(void)
{
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGINT);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_handler = &signal_handler;

	if (sigaction(SIGINT, &sa, NULL) != 0)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	if (sigaction(SIGTERM, &sa, NULL) != 0)
	{
		perror("sigaction");
		exit(EXIT_FAILURE);
	}
}

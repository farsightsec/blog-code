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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#include <pb_decode.h>
#include "src/nmsg.pb.h"

#define NMSG_MAGIC {'N', 'M', 'S', 'G'}
static const char magic[] = NMSG_MAGIC;

/* NMSG container is zlib compressed. */
#define NMSG_FLAG_ZLIB      0x01
/* NMSG container is fragmented. */
#define NMSG_FLAG_FRAGMENT  0x02

#define load_net16(buf, out)                        \
    do                                              \
    {                                               \
        uint16_t _my_16;                            \
        memcpy(&_my_16, buf, sizeof(uint16_t));     \
        _my_16 = ntohs(_my_16);                     \
        *(out) = _my_16;                            \
    }                                               \
    while (0)

uint32_t payloads = 0;
uint32_t payloads_total = 0;

bool count_payload_callback(pb_istream_t *stream, const pb_field_t *field,
        void **arg)
{
    nmsg_NmsgPayload payload = {};

    if (pb_decode(stream, nmsg_NmsgPayload_fields, &payload) == false)
    {
         return false;
    }

    payloads++;
    return true;
}

int main(int argc, char **argv)
{
    int fd, n;
    uint16_t vf;
    uint32_t processed;
    uint8_t *p = NULL, *buf = NULL;
    struct stat stat_buf;
    nmsg_Nmsg message;
    bool status;
    pb_istream_t stream;

    if (argc != 2)
    {
        printf("%s parse an NMSG file and emit container/payload count\n",
                argv[0]);
        printf("usage: %s file.nmsg\n", argv[0]);
        return EXIT_FAILURE;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "can't open %s\n", argv[1]);
        goto done;
    }
    if (fstat(fd, &stat_buf) == -1)
    {
        fprintf(stderr, "can't stat %s\n", argv[1]);
        goto done;
    }
    buf = malloc(stat_buf.st_size);
    if (buf == NULL)
    {
        fprintf(stderr, "can't malloc\n");
        goto done;
    }
    n = read(fd, buf, stat_buf.st_size);
    if (n != stat_buf.st_size)
    {
        fprintf(stderr, "can't read %s\n", argv[1]);
        goto done;
    }

    /* loop through file, processing as many containers as we find */
    for (payloads = processed = 0; processed < n; )
    {
        uint32_t p_len;

        p = buf + processed;

        /* confirm NMSG header */
        if (memcmp(p, magic, sizeof (magic)) != 0)
        {
            fprintf(stderr, "NMSG header verification failed\n");
            goto done;
        }
        p += 4;

        /* confirm NMSG protocol version */
        load_net16(p, &vf);
        if ((vf & 0xFF) != 2U)
        {
            fprintf(stderr, "NMSG version check failed\n");
            goto done;
        }

        /* we don't process compressed or fragmened payloads */
        if (vf >> 8 & NMSG_FLAG_ZLIB)
        {
            printf("NMSG container contains compressed payloads\n");
            goto done;
        }
        if (vf >> 8 & NMSG_FLAG_FRAGMENT)
        {
            printf("NMSG container contains fragmented payloads\n");
            goto done;
        }
        p += 2;

        /* get container length */
        p_len = ntohl(*((uint32_t* )(p)));
        p += 4;

        stream = pb_istream_from_buffer(p, p_len);

        message.payloads.funcs.decode = &count_payload_callback;
        status = pb_decode(&stream, nmsg_Nmsg_fields, &message);
        if (status == false)
        {
            fprintf(stderr, "decoding failed: %s\n", PB_GET_ERROR(&stream));
            goto done;
        }
#if DEBUG
        printf("%d byte NMSG container has %d payloads\n", p_len, payloads);
#endif
        processed += p_len + 10;
        payloads_total += payloads;
        payloads = 0;
    }
    printf("total payloads: %d\n", payloads_total);

done:
    if (fd)
    {
        close(fd);
    }
    if (buf)
    {
        free(buf);
    }
    return 0;
}

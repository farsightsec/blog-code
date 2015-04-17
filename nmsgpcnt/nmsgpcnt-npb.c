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

#include "common.h"

static uint32_t payloads = 0;
static uint32_t payloads_total = 0;

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
    ssize_t n;
    uint8_t *buf = NULL;
    uint32_t processed, containers;

    if (argc != 2)
    {
        printf("%s parse an NMSG file and emit container/payload count\n",
                argv[0]);
        printf("usage: %s file.nmsg\n", argv[0]);
        return EXIT_FAILURE;
    }

    buf = load_container(argv[1], &n);
    if (buf == NULL)
    {
        return EXIT_FAILURE;
    }

    /* loop through file, processing as many containers as we find */
    for (containers = payloads = processed = 0; processed < n; )
    {
        bool status;
        uint32_t c_len;
        uint8_t *p = NULL, version, flags;
        pb_istream_t stream;
        nmsg_Nmsg message = {};

        p = buf + processed;

        /* confirm NMSG header */
        if (CHECK_MAGIC(p) != 0)
        {
            fprintf(stderr, "NMSG header verification failed\n");
            break;
        }

        /* step over magic */
        p += 4;

        /* load version and flags */
        LOAD_VERSION_AND_FLAGS(p, version, flags);

        /* confirm NMSG protocol version */
        if (version != 2U)
        {
            fprintf(stderr, "NMSG version check failed\n");
            break;
        }

        /* we don't process compressed payloads */
        if (flags & NMSG_FLAG_ZLIB)
        {
            fprintf(stderr, "NMSG container contains compressed payloads\n");
            break;
        }

        /* we don't process fragmented payloads */
        if (flags & NMSG_FLAG_FRAGMENT)
        {
            fprintf(stderr, "NMSG container contains fragmented payloads\n");
            break;
        }

        /* step over version/flags */
        p += 2;

        /* get container length */
        c_len = ntohl(*((uint32_t* )(p)));

        /* step over length */
        p += 4;

        stream = pb_istream_from_buffer(p, c_len);

        message.payloads.funcs.decode = &count_payload_callback;
        status = pb_decode(&stream, nmsg_Nmsg_fields, &message);
        if (status == false)
        {
            fprintf(stderr, "decoding failed: %s\n", PB_GET_ERROR(&stream));
            goto done;
        }
        containers++;
        processed += c_len + 10;
        payloads_total += payloads;
        payloads = 0;
    }
    printf("containers:\t%d\n", containers);
    printf("payloads:\t%d\n", payloads_total);

done:
    if (buf)
    {
        free(buf);
    }
    return 0;
}

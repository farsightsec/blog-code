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

#include "src/nmsg.pb-c.h"

#include "common.h"

int main(int argc, char **argv)
{
    ssize_t n;
    uint8_t *buf = NULL;
    uint32_t payloads, processed, containers;

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
        goto done;
    }

    /* loop through file, processing as many containers as we find */
    for (containers = payloads = processed = 0; processed < n; )
    {
        uint16_t vf;
        uint32_t p_len;
        uint8_t *p = NULL;
        Nmsg__Nmsg *message;

        p = buf + processed;

        /* confirm NMSG header */
        CHECK_MAGIC(p);
        p += 4;

        /* confirm NMSG protocol version */
        CHECK_VERSION(p);

        /* we don't process compressed or fragmened payloads */
        CHECK_ZLIB(vf);

        /* we don't process compressed or fragmened payloads */
        CHECK_FRAG(vf);

        p += 2;

        /* get container size */
        GET_CONTAINER_SIZE(p, p_len);

        p += 4;

        message = nmsg__nmsg__unpack(0, p_len, p);
        if (message == NULL)
        {
            fprintf(stderr, "nmsg__nmsg__unpack() error");
            goto done;
        }
        containers++;
        processed += p_len + 10;
        payloads += message->n_payloads;
        nmsg__nmsg__free_unpacked(message, 0);
    }
    printf("containers:\t%d\n", containers);
    printf("payloads:\t%d\n", payloads);

done:
    if (buf)
    {
        free(buf);
    }
    return 0;
}

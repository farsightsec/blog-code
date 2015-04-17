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

uint8_t *load_container(char *filename, ssize_t *buf_size);

#define NMSG_MAGIC {'N', 'M', 'S', 'G'}
static const char magic[] = NMSG_MAGIC;

/* NMSG container is zlib compressed */
#define NMSG_FLAG_ZLIB      0x01
/* NMSG container is fragmented */
#define NMSG_FLAG_FRAGMENT  0x02

/* copy 16 bits from buf to out, ensure network byte order */
#define load_net16(buf, out)                        \
    do                                              \
    {                                               \
        uint16_t _my_16;                            \
        memcpy(&_my_16, buf, sizeof(uint16_t));     \
        _my_16 = ntohs(_my_16);                     \
        *(out) = _my_16;                            \
    }                                               \
    while (0)

/* confirm NMSG header */
#define CHECK_MAGIC(p)          memcmp(p, magic, sizeof (magic))

/* load NMSG version and flags from header */
#define LOAD_VERSION_AND_FLAGS(p, v, f)             \
    do                                              \
    {                                               \
        uint16_t vf;                                \
        load_net16(p, &vf);                         \
        v =  vf & 0xFF;                             \
        f =  vf >> 8;                               \
    }                                               \
    while (0)

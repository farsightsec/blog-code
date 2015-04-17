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

#include "./common.h"

/* protocol buffer constants */
#define PB_WIRETYPE_MASK    7   /* AND mask to get wire type bits */
#define PB_WT_VI            0   /* wire type: varint */
#define PB_WT_64            1   /* wire type: 64-bit */
#define PB_WT_LD            2   /* wire type: length delimited */
#define PB_WT_32            5   /* wire type: 32-bit */

/* protocol buffer Nmsg field numbers */
#define PB_NMSG_PAYLOAD     1   /* payload */
#define PB_NMSG_PAYLOAD_CRC 2   /* payload CRC */
#define PB_NMSG_SEQSRC      3   /* sequence source */
#define PB_NMSG_SEQID       4   /* sequence ID */

/* decode a varint and stick results in val */
#define CHNM_DCDE_VARINT(shifter, buf, val, idx)    \
    shifter = 0, val = 0;                           \
    do                                              \
    {                                               \
        idx++;                                      \
        val |= (buf[idx] & 0x7F) << shifter;        \
        shifter += 7;                               \
    }                                               \
    while (buf[idx] & 0x80);                        \

/* holds results of protobuf deserialization */
struct nmsg_pb_data
{
    uint32_t n_payloads;    /* number of payloads */
};
typedef struct nmsg_pb_data nmsg_pb_data_t;

int
decode_pb(const uint8_t *pb_data, uint32_t p_len, nmsg_pb_data_t *nmsg_data)
{
    int idx;
    uint8_t wire_type;
    uint64_t i, j;
    uint64_t shifter;

    memset(nmsg_data, 0, sizeof (*nmsg_data));

    i = 0;
    idx = 0;
    while (i < p_len)
    {
        /*
         *  Each key in the streamed message is a varint with the value 
         *  (field_number << 3) | wire_type.
         *  
         *  To decode a protobuf byte, AND off the lower 3 bits
         *  (GPB_WIRETYPE_MASK) to get the wire type. From there, right 
         *  shift by 3 to get the field number.
         *
         *  For an NMSG protobuf, the field numbers are:
         *
         *  message Nmsg
         *  {
         *      repeated NmsgPayload payloads     = 1;
         *      repeated uint32      payload_crcs = 2;
         *      optional uint32      sequence     = 3;
         *      optional uint64      sequence_id  = 4;
         *  }
         *
         *  And the protobuf wiretypes are:
         *
         *  0 Varint            PB_WT_VI
         *  1 64-bit            PB_WT_64
         *  2 length (payload)  PB_WT_LD
         *  5 32-bit            PB_WT_32
         */
        wire_type = pb_data[i] & PB_WIRETYPE_MASK;
        switch (wire_type)
        {
            case PB_WT_VI:
                CHNM_DCDE_VARINT(shifter, pb_data, j, i);
                i++;
                break;
            case PB_WT_64:
                /* next byte has the 64-bit value; skip both */
                i += 9;
                break;
            case PB_WT_LD:
                if ((pb_data[i] >> 3) == PB_NMSG_PAYLOAD)
                {
                    /* field number 1 == payload */
                    nmsg_data->n_payloads++;
                }
                CHNM_DCDE_VARINT(shifter, pb_data, j, i);
                /* skip over payload bytes */
                i += j;
                i++;
                break;
            case PB_WT_32:
                /* next byte has the 32-bit value; skip both */
                i += 5;
                break;
            default:
                /* unknown wire type */
                /* bad NMSG payload, possibly malfunctioning hardware? */
                return -1;
        }
    }
    return 1;
}

int main(int argc, char **argv)
{
    ssize_t n;
    uint32_t payloads, processed, containers;
    uint8_t *buf = NULL;

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
        int ret;
        uint16_t vf;
        uint32_t p_len;
        uint8_t *p = NULL;
        nmsg_pb_data_t nmsg_data;

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

        /* deserialize the protobuf encoded container */
        ret = decode_pb(p, p_len, &nmsg_data);
        if (ret < 0)
        {
            fprintf(stderr, "error: can't parse NMSG payload\n");
            goto done;
        }
        containers++;
        processed += p_len + 10;
        payloads += nmsg_data.n_payloads;
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

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

#define NMSG_MAGIC {'N', 'M', 'S', 'G'}
static const char magic[] = NMSG_MAGIC;

/* protocol buffer constants */
#define PB_WIRETYPE_MASK    7   /* AND mask to get wire type bits */
#define PB_WT_VI            0   /* wire type: varint */
#define PB_WT_64            1   /* wire type: 64-bit */
#define PB_WT_LD            2   /* wire type: length delimited */
#define PB_WT_32            5   /* wire type: 32-bit */

/* protocol buffer NMSG field numbers */
#define PB_NMSG_PAYLOAD     1   /* payload */
#define PB_NMSG_PAYLOAD_CRC 2   /* payload CRC */
#define PB_NMSG_SEQSRC      3   /* sequence source */
#define PB_NMSG_SEQID       4   /* sequence ID */

/* NMSG container is zlib compressed. */
#define NMSG_FLAG_ZLIB      0x01
/* NMSG container is fragmented. */
#define NMSG_FLAG_FRAGMENT  0x02

/* decode a varint and stick retulsts in val */
#define CHNM_DCDE_VARINT(shifter, buf, val, idx)    \
    shifter = 0, val = 0;                           \
    do                                              \
    {                                               \
        idx++;                                      \
        val |= (buf[idx] & 0x7F) << shifter;        \
        shifter += 7;                               \
    }                                               \
    while (buf[idx] & 0x80);                        \

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

/* holds results of PB deserialization */
struct nmsg_gpbd_data
{
    uint32_t n_payloads;    /* number of payloads */
    bool has_seqsrc;        /* has a seqsrc */
    uint32_t seqsrc;        /* seqsrc */
    bool has_seqid;         /* has a seqid */
    uint64_t seqid;         /* seqid */
};
typedef struct nmsg_gpbd_data nmsg_gpbd_data_t;

int
des(const uint8_t *pb_data, uint32_t p_len, nmsg_gpbd_data_t *nmsg_data)
{
    bool did_seqsrc, did_seqid;
    uint8_t wire_type;
    uint64_t i, j;
    uint64_t shifter;

    memset(nmsg_data, 0, sizeof (*nmsg_data));

    i = 0;
    did_seqid = false;
    did_seqsrc = false;
    while (i < p_len)
    {
        /*
         *  Each key in the streamed message is a varint with the value 
         *  (field_number << 3) | wire_type.
         *  
         *  To decode a GPB byte, AND off the lower 3 bits
         *  (GPB_WIRETYPE_MASK) to get the wire type. From there, right 
         *  shift by 3 to get the field number.
         *
         *  For an NMSG GPB, the field numbers are:
         *
         *  message Nmsg
         *  {
         *      repeated NmsgPayload payloads     = 1;
         *      repeated uint32      payload_crcs = 2;
         *      optional uint32      sequence     = 3;
         *      optional uint64      sequence_id  = 4;
         *  }
         *
         *  And the GPB wiretypes are:
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
                if ((pb_data[i] >> 3) == PB_NMSG_SEQSRC)
                {
                    nmsg_data->has_seqsrc = true;
                }
                else if ((pb_data[i] >> 3) == PB_NMSG_SEQID)
                {
                    nmsg_data->has_seqid = true;
                }
                CHNM_DCDE_VARINT(shifter, pb_data, j, i);
                if (nmsg_data->has_seqsrc == true && did_seqsrc == false)
                {
                    nmsg_data->seqsrc = j;
                    did_seqsrc = true;
                }
                if (nmsg_data->has_seqid == true && did_seqid == false)
                {
                    nmsg_data->seqid = j;
                    did_seqid = true;
                }
                i++;
                break;
            case PB_WT_64:
                i += 8;
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
                i += 4;
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
    int fd, n;
    uint16_t vf;
    uint32_t payloads, processed;
    uint8_t *p = NULL, *buf = NULL;
    struct stat stat_buf;
    nmsg_gpbd_data_t nmsg_data;

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

        /* deserialize the GPB encoded container */
        if (des(p, p_len, &nmsg_data) == -1)
        {
            fprintf(stderr, "GPB parse error\n");
            goto done;
        }
#if DEBUG
        printf("%d byte NMSG container has %d payloads\n", p_len,
                nmsg_data.n_payloads);
#endif
        processed += p_len + 10;
        payloads += nmsg_data.n_payloads;
    }
    printf("total payloads: %d\n", payloads);

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

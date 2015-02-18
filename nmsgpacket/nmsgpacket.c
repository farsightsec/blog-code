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

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <pcap.h>

#include <nmsg.h>
#include <nmsg/base/defs.h>


int
main(int argc, char **argv)
{
    int fd, rc, count;
    nmsg_io_t io;
    nmsg_res res;
    pcap_t *phandle;
    nmsg_pcap_t pcap;
    nmsg_msgmod_t mod;
    nmsg_input_t input;
    nmsg_output_t output;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (argc != 3 || ((count = atoi(argv[2])) < 1))
    {
        printf("%s capture packets and encode as NMSG base:packet\n", argv[0]);
        printf("usage: %s interface count\n", argv[0]);
        printf("  interface:\tlook for packets here\n");
        printf("  count:\tprocess this many payloads (positive integer)\n");
        return EXIT_FAILURE;
    }

    /* initialize nmsg */
    res = nmsg_init();
    if (res != nmsg_res_success)
    {
        fprintf(stderr, "nmsg_init(): failed: %s\n", nmsg_res_lookup(res));
        return EXIT_FAILURE;
    }

    /* initialize the nmsg io engine */
    io = nmsg_io_init();
    if (io == NULL)
    {
        fprintf(stderr, "nmsg_io_init(): failed\n");
        return EXIT_FAILURE;
    }

    /* get the message module for base:packet */
    mod = nmsg_msgmod_lookup(NMSG_VENDOR_BASE_ID, NMSG_VENDOR_BASE_PACKET_ID);
    if (mod == NULL)
    {
        fprintf(stderr, "nmsg_msgmod_lookup(): unknown msgmod\n");
        return EXIT_FAILURE;
    }

    /* initialize a pcap handle */
    phandle = pcap_create(argv[1], errbuf);
    if (phandle == NULL)
    {
        fprintf(stderr, "pcap_create (%s): %s\n", argv[1], errbuf);
        return EXIT_FAILURE;
    }

    /* set the pcap snapshot length to the nmsg recommended value */
    rc = pcap_set_snaplen(phandle, NMSG_DEFAULT_SNAPLEN);
    if (rc != 0)
    {
        fprintf(stderr, "pcap_set_snaplen() failed: %d\n", rc);
        return EXIT_FAILURE;
    }

    /* set the pcap read timeout to 1000ms */
    rc = pcap_set_timeout(phandle, 1000);
    if (rc != 0)
    {
        fprintf(stderr, "pcap_set_timeout() failed: %d\n", rc);
        return EXIT_FAILURE;
    }

    /* start packet capture */
    rc = pcap_activate(phandle);
    if (rc != 0)
    {
        fprintf(stderr, "pcap_activate() failed with error code: %d\n", rc);
        return EXIT_FAILURE;
    }

    /* initialize a new nmsg pcap input */
    pcap = nmsg_pcap_input_open(phandle);
    if (pcap == NULL)
    {
        fprintf(stderr, "nmsg_pcap_input_open() failed\n");
        return EXIT_FAILURE;
    }

    /* initialize a new NMSG pcap input from a pcap descriptor */
    input = nmsg_input_open_pcap(pcap, mod);
    if (input == NULL)
    {
        fprintf(stderr, "nmsg_input_open_pcap(): failed\n");
        return EXIT_FAILURE;
    }

    /* add an nmsg input object to the io engine */
    res = nmsg_io_add_input(io, input, NULL);
    if (res != nmsg_res_success)
    {
        fprintf(stderr, "nmsg_io_add_intput() failed: %s\n",
                nmsg_res_lookup(res));
        return EXIT_FAILURE;
    }

    /* open a new file to write binary NMSGs */
    fd = open("nmsgpacket.nmsg", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        fprintf(stderr, "open() failed: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    /* initialize a new binary file nmsg output */
    output = nmsg_output_open_file(fd, NMSG_WBUFSZ_MAX);
    if (output == NULL)
    {
        fprintf(stderr, "nmsg_output_open_file(): failed\n");
        return EXIT_FAILURE;
    }

    /* add an nmsg output object to the io engine */
    res = nmsg_io_add_output(io, output, NULL);
    if (res != nmsg_res_success)
    {
        fprintf(stderr, "nmsg_io_add_output() failed: %s\n",
                nmsg_res_lookup(res));
        return EXIT_FAILURE;
    }

    /* initialize a new stdout presentation format nmsg output */
    output = nmsg_output_open_pres(STDOUT_FILENO);
    if (output == NULL)
    {
        fprintf(stderr, "nmsg_output_open_pres(): failed\n");
        return EXIT_FAILURE;
    }

    /* add an nmsg output object to the io engine */
    res = nmsg_io_add_output(io, output, NULL);
    if (res != nmsg_res_success)
    {
        fprintf(stderr, "nmsg_io_add_output() failed: %s\n",
                nmsg_res_lookup(res));
        return EXIT_FAILURE;
    }

    /* configure the io engine to close after count payloads */
    nmsg_io_set_count(io, count);

    /* configure the io engine to mirror payloads to each output */
    nmsg_io_set_output_mode(io, nmsg_io_output_mode_mirror);

    /* start processing nmsg inputs and outputs */
    res = nmsg_io_loop(io);
    if (res != nmsg_res_success)
    {
        fprintf(stderr, "nmsg_io_loop() failed: %s\n", nmsg_res_lookup(res));
        return EXIT_FAILURE;
    }

    /* deallocate the resources associated with the io engine */
    nmsg_io_destroy(&io);

    return EXIT_SUCCESS;
}

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
#include <unistd.h>
#include <fcntl.h>

#include <err.h>

#include <nmsg.h>
#include <nmsg/base/dnsqr.pb-c.h>
#include <nmsg/base/defs.h>

#include <xs/xs.h>

nmsg_output_t filter_output;

int dnsqr_filter(nmsg_message_t);

static void
callback(nmsg_message_t msg, void *user) {
	int32_t vid, msgtype;

	vid = nmsg_message_get_vid(msg);
	msgtype = nmsg_message_get_msgtype(msg);
	if (vid == NMSG_VENDOR_BASE_ID &&
	    msgtype == NMSG_VENDOR_BASE_DNSQR_ID) {
		if (dnsqr_filter(msg))
			nmsg_output_write(filter_output, msg);
	}
	nmsg_message_destroy(&msg);
}

void err_nmsg_res(int eval, nmsg_res res, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, ": %s", nmsg_res_lookup(res));
	exit(eval);
}

int
main(int argc, char **argv) {
	nmsg_io_t io;
	nmsg_output_t output;
	nmsg_input_t input;
	nmsg_res res;
	void *xs_ctx = xs_init();
	char c;
	int fd;

	res = nmsg_init();
	if (res != nmsg_res_success) {
		fprintf(stderr, "unable to initialize libnmsg\n");
		return (EXIT_FAILURE);
	}

	io = nmsg_io_init();
	if (io == NULL) {
		fprintf(stderr, "nmsg_io_init() failed\n");
		return (EXIT_FAILURE);
	}

	while ( (c=getopt(argc, argv, "r:C:L:S:w:h")) != -1 ) {
		switch(c) {

		case 'r':
			fd = open(optarg, O_RDONLY);
			if (fd < 0)
				err(EXIT_FAILURE, "[-r %s] open() failed", optarg);
			input = nmsg_input_open_file(fd);
			if (input == NULL) 
				errx(EXIT_FAILURE, "[-r %s] nmsg_input_open_file() failed.", optarg);

			res = nmsg_io_add_input(io, input, NULL);
			if (res != nmsg_res_success)
				err_nmsg_res(EXIT_FAILURE, res, "[-r %s] nmsg_io_add_input() failed");
			break;

		case 'C':
			res = nmsg_io_add_input_channel(io, optarg, NULL);
			if (res != nmsg_res_success)
				err_nmsg_res(EXIT_FAILURE, res, "[-C %s] nmsg_io_add_input_channel(%s) failed", optarg);

			break;
		case 'L':
			input = nmsg_input_open_xs_endpoint(xs_ctx, optarg);
			if (input == NULL) 
				errx(EXIT_FAILURE, "[-L %s] nmsg_input_open_xs_endpoint() failed.", optarg);
			res = nmsg_io_add_input(io, input, NULL);
			if (res != nmsg_res_success)
				err_nmsg_res(EXIT_FAILURE, res, "[-L %s] nmsg_io_add_input() failed.", optarg);
			break;

		case 'S':
			filter_output = nmsg_output_open_xs_endpoint(xs_ctx, optarg, NMSG_WBUFSZ_JUMBO);
			if (filter_output == NULL)
				errx(EXIT_FAILURE, "[-S %s] nmsg_output_open_xs_endpoint() failed.", optarg);
			break;

		case 'w': 
			fd = open(optarg, O_WRONLY|O_CREAT, 0666);
			if (fd < 0)
				err(EXIT_FAILURE, "[-w %s] open() failed", optarg);
			filter_output = nmsg_output_open_file(fd, NMSG_WBUFSZ_MAX);
			if (filter_output == NULL) 
				errx(EXIT_FAILURE, "[-w %s] nmsg_output_open_file() failed", optarg);
			break;

		case '?':
		case 'h':
			fprintf(stderr, "Usage: %s\n"
				"[-h]		Display help text\n"
				"[-r file]	Read nmsg data from file\n"
				"[-L xep]	Read nmsg data from XS endpoint\n"
				"[-C channel]	Read nmsg data from channel\n"
				"[-w file]	Write nmsg data to file\n"
				"[-S xep]	Write nmsg data to XS endpoint\n",
				argv[0]);
			exit(1);
		}
	}

	if (filter_output == NULL)
		errx(EXIT_FAILURE, "No outputs given.");

	output = nmsg_output_open_callback(callback, NULL);
	if (output == NULL)
		errx(EXIT_FAILURE, "nmsg_output_open_callback() failed.");

	res = nmsg_io_add_output(io, output, NULL);
	if (res != nmsg_res_success)
		err_nmsg_res(EXIT_FAILURE, res, "nmsg_io_add_output() failed");

	res = nmsg_io_loop(io);
	if (res != nmsg_res_success) 
		err_nmsg_res(EXIT_FAILURE, res, "nmsg_io_loop() failed");

	nmsg_io_destroy(&io);

	return (EXIT_SUCCESS);
}

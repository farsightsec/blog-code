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

#include <nmsg.h>
#include <wdns.h>

int
dnsqr_filter(nmsg_message_t msg) {
	nmsg_res nres;
	wdns_res wres;

	wdns_message_t resp = {0}, req = {0};
	uint8_t *data;
	uint16_t *rcode;
	size_t len;
	int result = 0;

	nres = nmsg_message_get_field(msg, "rcode", 0, (void **)&rcode, NULL);
	if (nres != nmsg_res_success) return result;

	nres = nmsg_message_get_field(msg, "response", 0, (void **)&data, &len);
	if (nres != nmsg_res_success) return result;

	wres = wdns_parse_message(&resp, data, len);
	if (wres != wdns_res_success) return result;

	if (!resp.edns.present) {
		nres = nmsg_message_get_field(msg, "request", 0,
						(void **)&data, &len);
		if (nres == nmsg_res_success) {
			wres = wdns_parse_message(&req, data, len);
			if (wres == wdns_res_success) 
				result = req.edns.present;
		}
	}

	wdns_clear_message(&req);
	wdns_clear_message(&resp);
	return result;
}

# dnsqr_filter
A reusable example of an [`nmsg`](https://github.com/farsightsec/nmsg) filter
using [`wdns`](https://github.com/farsightsec/wdns) to filter interesting
DNS traffic out of a passive DNS stream. Originally published in
[Processing Passive DNS With the nmsg C API](https://www.farsightsecurity.com/Blog/20150318-cmikk-processing-pdns/).

The filter included in `edns_filter.c` looks for non-`EDNS0` responses
to `EDNS0` queries, which indicates an authoritative server not complying
with `EDNS0`.

New filters can be added by defining a function matching:
```C
	int dnsqr_filter(nmsg_message_t);
```
in a new file `your_filter.c`, and specifying rules to make
`dnsqr_your_filter` in `Makefile`. If `dnsqr_filter(m)` returns
nonzero, `m` is copied to the output file or socket given on
the `dnsqr_your_filter` command line.

## dnsqr_filter example

    $ ./dnsqr_edns_filter -h
    Usage: ./dnsqr_edns_filter
    [-h]        Display help text
    [-r file]   Read nmsg data from file
    [-l ip/port]    Read nmsg data datagram socket
    [-C channel]    Read nmsg data from channel
    [-w file]   Write nmsg data to file
    [-s ip/port]    Write nmsg data to datagram socket

    $ ./dnsqr_edns_filter -C ch202 -w edns0_fail.nmsg
    ^C

    $ nmsgtool -r edns0_fail.nmsg -c 1
    [241] [2015-03-17 20:40:00.940989000] [1:9 base dnsqr] [4ff49032] [] [] 
    type: UDP_QUERY_RESPONSE
    query_ip: xx.xx.xx.xx
    response_ip: yy.yy.yy.yy
    proto: UDP (17)
    query_port: 10718
    response_port: 53
    id: 11333
    qname: www..com.
    qclass: IN (1)
    qtype: A (1)
    rcode: NOERROR (0)
    delay: 0.085448
    udp_checksum: CORRECT
    query: [48 octets]
    ;; ->>HEADER<<- opcode: QUERY, rcode: NOERROR, id: 11333
    ;; flags:; QUERY: 1, ANSWER: 0, AUTHORITY: 0, ADDITIONAL: 0

    ;; QUESTION SECTION:
    ;www..com. IN A

    ;; ANSWER SECTION:

    ;; AUTHORITY SECTION:

    ;; ADDITIONAL SECTION:
    ---
    response: [53 octets]
    ;; ->>HEADER<<- opcode: QUERY, rcode: NOERROR, id: 11333
    ;; flags: qr aa; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 0

    ;; QUESTION SECTION:
    ;www..com. IN A

    ;; ANSWER SECTION:
    www..com. 300 IN A zz.zz.zz.zz

    ;; AUTHORITY SECTION:

    ;; ADDITIONAL SECTION:
    ---

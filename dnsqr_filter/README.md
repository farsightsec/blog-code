# dnsqr_filter

A reusable example of an [`nmsg`](https://github.com/farsightsec/nmsg) filter
using [`wdns`](https://github.com/farsightsec/wdns) to filter interesting
`base`:`dnsqr` messages out of a passive DNS stream.

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

## example

    $ nmsgtool -l 127.0.0.1/5353 -w ... &
    $ ./dnsqr_edns_filter -C ch202 -s 127.0.0.1/5353

# dnsqr_filter

A resuable example of how to use the [`nmsg`](https://github.com/farsightsec/nmsg) and [`wdns`](https://github.com/farsightsec/wdns) APIs to filter interesting tidbits out of a passive DNS stream.

## example

    $ nmsgtool -l 127.0.0.1/5353 -w ... &
    $ ./dnsqr_edns_filter -C ch202 -s 127.0.0.1/5353

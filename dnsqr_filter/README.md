# dnsqr_filter

A resuable example of how to use the [`nmsg`](https://github.com/farsightsec/nmsg) and [`wdns`](https://github.com/farsightsec/wdns) APIs to filter interesting tidbits out of a passive DNS stream.

## example

    $ ./dnsqr_edns_filter -C ch202 -S ipc:///tmp/noedns.sock,pubsub,accept &
    $ nmsgtool -L ipc:///tmp/noedns.sock,pubsub,connect -c 100

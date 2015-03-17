# blog-code
The code published in [Farsight Security Blog](https://www.farsightsecurity.com/Blog) articles is made available here for easy download.

## [nmsgpacket](nmsgpacket)
A simple example of how to use the [`nmsg`](https://github.com/farsightsec/nmsg)
API. It reads packets from a network interface, converts them to NMSG, and
mirrors the payloads to a binary file and to stdout. Originally published in
[Farsight Security's Network Message, Volume 4: The C Programming API](https://www.farsightsecurity.com/Blog/20150218-mschiffm-nmsg-api-c/).

## [pynmsgpacket](pynmsgpacket)
A simple example of how to use the
[`pynmsg`](https://github.com/farsightsec/pynmsg) API. It reads
`base`:`packet` encoded NMSGs, winnows to IPv4 TCP or UDP packets and prints to
stdout a 5-tuple of source IP, source port, destination IP, destination port,
protocol. If configured to do so, it will also construct `base`:`ipconn`
encoded NMSGs and send to them to a network socket. It was originally published
in
[Farsight Security's Network Message, Volume 5: The Python Programming API](https://www.farsightsecurity.com/Blog/20150225-mschiffm-nmsg-api-python/).

## [dnsqr_filter](dnsqr_filter)
An example of how to use the [`nmsg`](https://github.com/farsightsec/nmsg)
and [`wdns`](https://github.com/farsightsec/wdns) APIs to filter interesting
DNS traffic out of a raw passive DNS feed.

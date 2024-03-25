# blog-code
The code published in [Farsight TXT Record Blog](https://www.domaintools.com/resources/blog/?_resources_tags=farsight-txt-record) articles is made available here for easy download.

## [dnsqr_filter](dnsqr_filter)
A reusable example of an [`nmsg`](https://github.com/farsightsec/nmsg) filter
using [`wdns`](https://github.com/farsightsec/wdns) to filter interesting
DNS traffic out of a passive DNS stream. Originally published in
[Processing Passive DNS With the nmsg C API](https://www.domaintools.com/resources/blog/processing-passive-dns-with-the-nmsg-c-api/).

## [ipv4map](ipv4map)
A set of golang tools for collecting and mapping IPv4 addresses. Originally
published in [Visualizing the incidence of IPv4 addresses in Farsight's DNSDB](https://www.domaintools.com/resources/blog/visualizing-the-incidence-of-ipv4-addresses-in-farsights-dnsdb/).

## [nmsgjsonseqsrc](nmsgjsonseqsrc)
A simple example of how to use the
[`nmsg`](https://github.com/farsightsec/nmsg) IO engine, callback interface, and seqsrc container tracking feature set. Originally published in
[Farsight Security's Network Message: Loss Tracking Explained](https://www.domaintools.com/resources/blog/farsights-network-message-loss-tracking-explained/).

## [nmsgpacket](nmsgpacket)
A simple example of how to use the [`nmsg`](https://github.com/farsightsec/nmsg)
API. It reads packets from a network interface, converts them to NMSG, and
mirrors the payloads to a binary file and to stdout. Originally published in
[Farsight Security's Network Message, Volume 4: The C Programming API](https://www.domaintools.com/resources/blog/farsights-network-message-volume-4-the-c-programming-api/).

## [nmsgpcnt](nmsgpcnt)
An example of how manually deserialize an
[`NMSG`](https://github.com/farsightsec/nmsg) [Google Protocol Buffer](https://developers.google.com/protocol-buffers/) message using the C programming
language. It reads NMSGs from a file and counts the number of containers and
payloads. Two other examples are presented ([`protoc-c`](https://github.com/protobuf-c/protobuf-c) and [`Nanopb`](http://koti.kapsi.fi/jpa/nanopb/) to help with
performance benchmarking. Originally published in
[Google Protocol Buffer Deserialization The Hard Way](https://www.domaintools.com/resources/blog/google-protocol-buffer-deserialization-the-hard-way/).

## [pynmsgpacket](pynmsgpacket)
A simple example of how to use the
[`pynmsg`](https://github.com/farsightsec/pynmsg) API. It reads
`base`:`packet` encoded NMSGs, winnows to IPv4 TCP or UDP packets and prints to
stdout a 5-tuple of source IP, source port, destination IP, destination port,
protocol. If configured to do so, it will also construct `base`:`ipconn`
encoded NMSGs and send to them to a network socket. It was originally published
in
[Farsight Security's Network Message, Volume 5: The Python Programming API](https://www.domaintools.com/resources/blog/farsights-network-message-volume-5-the-python-programming-api/).

## [scaladnsdb](scaladnsdb)
An example of how to build a demo GUI front end for our DNSDB API in Scala. It was
originally published in
[Building a Demo GUI Front End For DNSDB API In Scala With Swing For The Mac and For Windows PCs](https://www.domaintools.com/resources/blog/new-farsight-white-paper-available-for-download-building-a-demo-gui-front-end-for-dnsdb-api-in-scala/).

## [scaladnsdb2](scaladnsdb2)
An example of how to build a GUI front end for our DNSDB API to perform ASN queries in Scala. It was originally published in
[Automatically Going From An Autonomous System Number (ASN), To The Prefixes It Announces, To The Domain Names Seen Using Those IPs: Another Farsight DNSDB API Demo Application and Whitepaper](https://www.domaintools.com/resources/blog/automatically-going-from-an-autonomous-system-number-asn-to-the-prefixes-it-announces-to-the-domain-names-seen-using-those-ips-another-farsight-dnsdb-api-demo-application-and-whitepaper/).

## [scaladnsdb3](scaladnsdb3)
An update to ASN-to-prefixes-to-domains article, showing use of `stat.ripe.net` as an alternative to `bgpview.io`. It was originally published in
[ASN-To-Prefixes-To-Domain Names -- Update to May 2017 Blog Article](https://www.domaintools.com/resources/blog/asn-to-prefixes-to-domain-names-update-to-may-2017-blog-article/).

## [sie_get](sie_get)
Simple SIE Batch command line client in C illustrating use of the SIE Batch API (from the March 2020 blog).

## [sie_get_clients](sie_get_clients)
Code from the "Working With The SIE Batch API: A Command Line Client In Ruby, Perl, Python and C" whitepaper.

## [sratesttool](sratesttool)
Hello World, AXA-style. Introductory example of how to use the `libaxa` C API.
`sratesttool` connects to an SRA server, sets a watch, enables a
channel and streams watch hits to the console. Originally published in
[Farsight's Advanced Exchange Access: The C Programming API, Part One](https://www.domaintools.com/resources/user-guides/the-axa-api-reference/).

## [bang_question](bang_question)
Sometimes you'll be rolling along when "BANG!" -- something flys in from left field and hits a system. The natural reaction is to ask, "What was THAT?"
This application's name seems to perfectly capture that "!?" paradigm. This is a demonstration 9 tab Python3 GUI application leveraging Tkinter. 

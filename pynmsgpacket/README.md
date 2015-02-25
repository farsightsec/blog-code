# pynmsgpacket
A simple example of how to use the [`pynmsg`](https://github.com/farsightsec/pynmsg)API. It reads `base`:`packet` encoded NMSGs, winnows to IPv4 TCP or UDP
packets and prints to stdout a 5-tuple of source IP, source port, destination
IP, destination port, protocol). If configured to do so, it will also construct
`base`:`ipconn` encoded NMSGs and send to them to a network socket. It was
originally published in
[Farsight Security's Network Message, Volume 5: The Python Programming API](https://www.farsightsecurity.com/Blog/20150225-mschiffm-nmsg-api-python/).

## pynmsgpacket example
To wit:

    $ ./pynmsgpacket/pynmsgpacket 
    usage: pynmsgpacket [-h] [-c COUNT] [-s SOCKET] [input]

    Print IPv4 TCP or UDP packet data from base:packet encoded NMSGs and
    optionally write base:ipconn NMSGs to a remote listener

    positional arguments:
      input                 input file, also accepts input from pipeline

    optional arguments:
      -h, --help            show this help message and exit
      -c COUNT, --count COUNT
                            stop after count payloads
      -s SOCKET, --socket SOCKET
                            write binary NMSGs to socket (i.e., 127.0.0.1/9430)

Process a few payloads from a previously created `nmsgpacket.nmsg` file:

    $ pynmsgpacket -c 3 nmsgpacket.nmsg
    [2015-02-22 06:58:27.573210000] [1:12 base packet] [] [] []
    10.0.1.52.17500 --> 255.255.255.255.17500 (17)
    [2015-02-22 06:58:27.573489000] [1:12 base packet] [] [] []
    10.0.1.52.17500 --> 10.0.1.255.17500 (17)
    [2015-02-22 06:58:30.32007000] [1:12 base packet] [] [] []
    172.16.82.195.80 --> 10.0.1.51.59451 (6)

More complex command-line invocations are available as well. First, instantiate
an `nmsgtool` listener:

    $ nmsgtool -l 10.0.1.52/9430

Then invoke `nmsgtool` to read from the network and pipe NMSGs to
`pynmsgpacket` which will dump the 5-tuple to stdout and then emit
`base`:`ipconn` NMSGs to the remote listener:

    $ nmsgtool -i en0 -V base -T packet -w - --unbuffered | pynmsgacket -s 10.0.1.52/9430
    [2015-02-22 20:54:06.253410000] [1:12 base packet] [00000000] [] []
    10.0.1.51.61929 --> 10.0.1.52.22 (6)
    [2015-02-22 20:54:06.253410000] [1:12 base packet] [00000000] [] []
    10.0.1.51.61929 --> 10.0.1.52.22 (6)
    [2015-02-22 20:54:06.253410000] [1:12 base packet] [00000000] [] []
    10.0.1.51.61929 --> 10.0.1.52.22 (6)
    [2015-02-22 20:54:06.253418000] [1:12 base packet] [00000000] [] []
    10.0.1.51.61929 --> 10.0.1.52.22 (6)
    ...

And the following is emitted by the original `nmsgtool` listener:

    [20] [2015-02-22 20:54:07.143420934] [1:5 base ipconn] [00000000] [] []
    proto: 6
    srcip: 10.0.1.51
    srcport: 61929
    dstip: 10.0.1.52
    dstport: 22

    [20] [2015-02-22 20:54:07.143464088] [1:5 base ipconn] [00000000] [] []
    proto: 6
    srcip: 10.0.1.51
    srcport: 61929
    dstip: 10.0.1.52
    dstport: 22

    [20] [2015-02-22 20:54:07.143507957] [1:5 base ipconn] [00000000] [] []
    proto: 6
    srcip: 10.0.1.51
    srcport: 61929
    dstip: 10.0.1.52
    dstport: 22

    [20] [2015-02-22 20:54:07.143552064] [1:5 base ipconn] [00000000] [] []
    proto: 6
    srcip: 10.0.1.51
    srcport: 61929
    dstip: 10.0.1.52
    dstport: 22

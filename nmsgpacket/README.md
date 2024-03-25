# nmsgpacket
A simple example of how to use the [`nmsg`](https://github.com/farsightsec/nmsg)API. It reads packets from a network interface, converts them to NMSG, and
mirrors the payloads to a binary file and to stdout. Originally published in
[Farsight Security's Network Message, Volume 4: The C Programming API](https://www.domaintools.com/resources/blog/farsights-network-message-volume-4-the-c-programming-api/).

## nmsgpacket example
To wit:

    $ ./nmsgpacket
    ./nmsgpacket capture packets and encode as NMSG base:packet
        usage: ./nmsgpacket interface count
        interface: look for packets here
        count: process this many payloads (positive integer)

    $ ./nmsgpacket en0 5
    [159] [2015-02-16 20:42:47.295350000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=154>

    [159] [2015-02-16 20:42:47.296414000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=154>

    [82] [2015-02-16 20:42:48.831159000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=78>

    [333] [2015-02-16 20:42:48.832891000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=328>

    [82] [2015-02-16 20:42:49.138423000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=78>

    $ nmsgtool -r nmsgpacket.nmsg 
    [159] [2015-02-16 20:42:47.295350000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=154>

    [159] [2015-02-16 20:42:47.296414000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=154>

    [82] [2015-02-16 20:42:48.831159000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=78>

    [333] [2015-02-16 20:42:48.832891000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=328>

    [82] [2015-02-16 20:42:49.138423000] [1:12 base packet] [00000000] [] [] 
    payload_type: IP
    payload: <BYTE ARRAY LEN=78>

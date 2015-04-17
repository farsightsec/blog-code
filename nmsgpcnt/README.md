# nmsgpcnt
This is `nmsgpcnt`. It contains code that counts NMSG protobuf payloads. It is
intended as an example of how to write C code to manually deserialize an NMSG
Google Protocol Buffer encoded container. It reads input from a binary NMSG
file and counts the number of NMSG containers and payloads. Originally
published in [Google Protocol Buffer Deserialization The Hard Way](https://www.farsightsecurity.com/Blog/20150417-mschiffm-nmsg-protobuf-deserialize/).

In addition to the primary `nmsgpcnt-fsi` program, two other reference
implementations of `nmsgpcnt` are included. One built using the
[protoc-c](https://github.com/protobuf-c/protobuf-c) API, the other using the
[nanopb](http://koti.kapsi.fi/jpa/nanopb/) API. To compile these tools you'll
need to install both packages.

## File Manifest
`nmsgpcnt` comes with three top-level source files containing the program code
to count NMSG containers payloads. While the FSI implementation is
self-contained, the other two examples require respective API code. It can be
confusing so here's a file manifest:

* `Makefile`:             Simple non-autoconf'd Makefile
* `README.md`:            This file
* `nmsgpcnt-fsi.c`        Farsight Security Nmsg protobuf decoder
* `nmsgpcnt-npb.c`        Nanopb Nmsg protobuf decoder
* `nmsgpcnt-ptc.c`        Protoc-c Nmsg protobuf decoder
* `src/nmsg.pb-c.{c,h}`   Protoc-c generated Nmsg interface
* `src/nmsg.pb.{c,h}`     Nanopb Nmsg generated definitions and header
* `src/nmsg.proto`        Nmsg protobuf definitions
* `src/pb.h`              Nanopb low-level header
* `src/pb_common.{c,h}`   Nanopb common support functions
* `src/pb_decode.{c,h}`   Nanopb runtime library decoding routines
* `test.sh`               Benchmarking test script

## nmsgpcnt examples
All of the `nmsgpcnt` programs have the same calling convention:

    $ nmsgpcnt-fsi
    nmsgpcnt-npb parse an NMSG file and emit container/payload count
    usage: ./nmsgpcnt-npb file.nmsg

For SIE / SRA customers you can easily generate a file to use via `nmsgtool`.

    $ nmsgtool -C ch202 -c 2000000 -w 202-2000000.nmsg

The included `test.sh` script will invoke and time all three programs against
the specified input file:

    $ ./test.sh 202-2000000.nmsg
    Running nmsgpcnt-npb (nanopb) against 202-2000000.nmsg
    containers: 720
    payloads:   2000481
    real 2.31
    user 1.84
    sys 0.47
    Running nmsgpcnt-ptc (protoc-c) against 202-2000000.nmsg
    containers: 720
    payloads:   2000481
    real 1.47
    user 0.97
    sys 0.49
    Running nmsgpcnt-fsi (Farsight Security) against 202-2000000.nmsg
    containers: 720
    payloads:   2000481
    real 0.59
    user 0.15
    sys 0.44

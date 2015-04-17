#!/bin/bash

if [ $# != 1 ]
then
    echo "Usage: $0 file.nmsg"
    echo "file.nsmg is a file containing NMSGs"
    exit 0
fi

if [ ! -f ./nmsgpcnt-npb ]; then
    echo "nmsgpcnt-npb doesn't exist, building it"
    make nmsgpcnt-npb
fi
echo "Running nmsgpcnt-npb (nanopb) against $1"
time -p ./nmsgpcnt-npb $1

if [ ! -f ./nmsgpcnt-ptc ]; then
    echo "nmsgpcnt-ptc doesn't exist, building it"
    make nmsgpcnt-ptc
fi
echo "Running nmsgpcnt-ptc (protoc-c) against $1"
time -p ./nmsgpcnt-ptc $1

if [ ! -f ./nmsgpcnt-fsi ]; then
    echo "nmsgpcnt-fsi doesn't exist, building it"
    make nmsgpcnt-fsi
fi
echo "Running nmsgpcnt-fsi (Farsight Security) against $1"
time -p ./nmsgpcnt-fsi $1

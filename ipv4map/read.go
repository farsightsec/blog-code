//Copyright Farsight Security, Inc. 2017

/*
Reads the bit-per-address data-file and generates a
list of dotted-quad IP address for either each one or zero.
Called:
  read <filename> <mode>
Where:
  <filename> - is a file populated by setup
  <mode> - is either "one" or "zero", If "one" the output is
  a list of IP addresses with bits set to one. If "zero" the
  output is a list of IP addresses with bits set to Zero

For a quick progress check, hexdump works fine.
*/
package main

import (
	"encoding/binary"
	"fmt"
	"log"
	"net"
	"os"
	"syscall"
	"unsafe"
)

const addresses = 4294967295
const bytes = addresses / 8

func int2ip(nn uint32) net.IP {
	ip := make(net.IP, 4)
	binary.BigEndian.PutUint32(ip, nn)
	return ip
}

func openFile(filename string, flags int) *os.File {
	f, err := os.OpenFile(filename, flags, 0644)
	if err != nil {
		log.Fatal(err)
	}
	return f
}

func usage() {
	fmt.Fprintf(os.Stderr, "Usage\n read <filename> <mode> - Generate output.")
	fmt.Fprintf(os.Stderr, "\tmode is one of 'one' or 'zero' - one is IP is in use, 'zero' is not")
}

func main() {
	var mode byte
	t := int(unsafe.Sizeof(0)) * bytes

	log.SetFlags(0) // turn off timestamps
	if len(os.Args) < 3 {
		usage()
		os.Exit(1)
	}

	f := openFile(os.Args[1], os.O_RDWR)
	modestring := os.Args[2]

	if modestring == "one" {
		mode = 1
	} else if modestring == "zero" {
		mode = 0
	} else {
		usage()
		os.Exit(1)
	}

	mmap, err := syscall.Mmap(int(f.Fd()), 0, int(t), syscall.PROT_READ|syscall.PROT_WRITE, syscall.MAP_SHARED)
	if err != nil {
		log.Fatal(err)
	}

	for i := 0; i < bytes; i++ {
		now := mmap[i]
		k := i << 3
		j := 0
		for j < 8 {
			// copy; mask all but the first bit
			innernow := now
			innernow &= 0x01
			if innernow == mode {
				//If true calculate and emit.
				ip := (int2ip(uint32(k) + uint32(j)).String()) // + "/32")
				fmt.Println(ip)
			}
			//shift right.
			now = now >> 1
			j = j + 1
		}
	}

	if err != nil {
		log.Fatal(err)
	}
	err = f.Close()
	if err != nil {
		log.Fatal(err)
	}
}

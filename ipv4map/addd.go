//Copyright Farsight Security, Inc. 2017

/*
addd - add daemon
starts a simple TCP listener, connections send
new-line seperated streams of IP addresses. Those
addresses are updated in the provided data file.

Called:
  addd <filename> <port>
Where:
  <filename> - Name of the data file created with setup
  <port> - TCP port number to accept connections on.
*/
package main

import (
	"bufio"
	"encoding/binary"
	"log"
	"net"
	"os"
	"strings"
	"syscall"
	"unsafe"
)

const addresses = 4294967295
const bytes = addresses / 8

func openFile(filename string, flags int) *os.File {
	f, err := os.OpenFile(filename, flags, 0644)
	if err != nil {
		log.Fatal(err)
	}
	return f
}

func serve(listener net.Listener, handler func(net.Conn)) {
	i := 0
	for {
		client, err := listener.Accept()
		if client == nil {
			log.Println("couldn't accept: ", err)
			continue
		}
		i++
		log.Printf("%d: %v <-> %v\n", i, client.LocalAddr(), client.RemoteAddr())
		go handler(client)
	}
}

func handleConn(client net.Conn, ips chan<- string) {
	b := bufio.NewReader(client)
	defer client.Close()
	for {
		line, err := b.ReadBytes('\n')
		if err != nil { // EOF, or worse
			break
		}
		t := strings.TrimSpace(string(line))
		ips <- t
	}
}

func run(ips <-chan string, mmap []byte) {
	for ip := range ips {
		parsed := net.ParseIP(ip).To4()
		if len(parsed) == 4 {
			intIP := binary.BigEndian.Uint32(parsed)
			netInt := intIP >> 3
			ipInBlock := intIP & 0x07
			ipInt := 1 << ipInBlock
			now := mmap[netInt]
			now |= byte(ipInt)
			mmap[netInt] = now
			continue
		} else {
			log.Printf("Invalid IPv4 address '%s'", ip)
		}
	}
}

func main() {
	log.SetFlags(0) // turn off timestamps
	t := int(unsafe.Sizeof(0)) * bytes

	if len(os.Args) < 3 {
		log.Fatal("Usage\n addd <filename> <port> - Start the add daemon on port.")
	}

	f := openFile(os.Args[1], os.O_RDWR)

	mmap, err := syscall.Mmap(int(f.Fd()), 0, int(t), syscall.PROT_READ|syscall.PROT_WRITE, syscall.MAP_SHARED)
	if err != nil {
		log.Fatal(err)
	}

	add := make(chan string)
	go run(add, mmap)

	server, err := net.Listen("tcp", ":"+os.Args[2])
	if server == nil {
		log.Fatalf("couldn't start listening: %v", err)
	}
	defer syscall.Munmap(mmap)
	defer f.Close()

	serve(server, func(c net.Conn) {
		handleConn(c, add)
	})
}

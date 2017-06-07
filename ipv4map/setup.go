//Copyright Farsight Security, Inc. 2017

/*
Command for building a bit-per-address file for IPv4.
Called with:
  setup <filename>

Output files are 512MB in size.
*/
package main

import (
	"log"
	"os"
)

const addresses = 4294967295
const bytes = addresses / 8

// Generates a data-file given a filename
func genFile(file string) {
	f, err := os.Create(file)
	defer f.Close()
	if err != nil {
		log.Fatal(err)
	}

	if err := f.Truncate(bytes); err != nil {
		log.Fatal(err)
	}
}

func main() {
	log.SetFlags(0) // turn off timestamps
	if len(os.Args) < 2 {
		log.Fatal("Usage\n\n setup <filename> - Generate a blank data file")
	}
	log.Println("Generating empty file: ", os.Args[1])
	filename := os.Args[1]
	genFile(filename)
}

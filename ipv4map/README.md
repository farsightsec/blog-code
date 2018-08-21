# IPv4map
This is `ipv4map`, a set of tools for collecting/mapping IPv4 addresses. It
uses a data-file that assigns each IPv4 address to a bit-map. Output can be
generated as a IP per line list that can be feed to tools like
[ipv4-heatmap](https://github.com/hrbrmstr/ipv4-heatmap).

Originally published in [Visualizing the incidence of IPv4 addresses in Farsight's DNSDB](https://www.farsightsecurity.com/2017/06/07/bapril-ip-mapping)..

## Installation
  1. Clone the git repo: `git clone https://github.com/farsightsec/blog-code/tree/master/ipv4map`
  2. Compile the programs: `go build setup.go ; go build read.go; go build addd.go`

## Usage
First create an empty data file:

```
$ ./setup data.dat
Generating empty file: data.dat
$ ls -la data.dat
-rw-r--r--  1 bapril  staff  536870911 Feb 16 15:23 data.dat
$ hexdump data.dat
0000000 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*
1ffffff0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
1fffffff
```

Now start the add daemon (addd), giving it a data-file and a port number. The
data-file does not have to be empty, addresses will be set to one as they are
sent to addd, if an address is already one it is a noop.

```
$ ./addd data.dat 4444
```

In another terminal send some data to the daemon ([netcat](http://nc110.sourceforge.net/) is handy for this):
```
$ echo '0.0.0.0' | nc localhost 4444
$ hexdump data.dat
0000000 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0000010 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*
1ffffff0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
1fffffff

$ echo '0.0.0.1' | nc localhost 4444

$ hexdump data.dat
0000000 03 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0000010 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
*
1ffffff0 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
1fffffff

```
Read the data:

There are two read modes "one" and "zero". Mode "one" will list the IPv4
addresses that set to one, "zero" will list the addresses that are not set.

```
$ ./read data.dat one
0.0.0.0
0.0.0.1
$ ./read data.dat zero | head
0.0.0.2
0.0.0.3
0.0.0.4
0.0.0.5
0.0.0.6
0.0.0.7
0.0.0.8
0.0.0.9
0.0.0.10
0.0.0.11
$
```

The output of the read command can be piped into any number of other tools
including [ipv4-heatmap](https://github.com/hrbrmstr/ipv4-heatmap).

## Contributing
 1. Fork it!
 2. Create your feature branch: `git checkout -b my-new-feature`
 3. Commit your changes: `git commit -am 'Add some feature'`
 4. Push to the branch: `git push origin my-new-feature`
 5. Submit a pull request

## Credits
 * Chris Mekkelson - Review/improvements
 * Joe StSauver - Review

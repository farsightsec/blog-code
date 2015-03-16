OBJS=main.o edns_filter.o
TARGETS=dnsqr_edns_filter

default: dnsqr_edns_filter

dnsqr_edns_filter: main.o edns_filter.o
	$(CC) $(CFLAGS) -o $@ main.o edns_filter.o -lnmsg -lwdns -lxs

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGETS)

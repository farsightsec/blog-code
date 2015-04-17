CFLAGS += -Wall -Werror -g -O0 -Isrc/ -I.
NMSGPCNTSRC = nmsgpcnt-npb.c src/nmsg.pb.c src/pb_decode.c src/pb_common.c common.c

all: nmsgpcnt-fsi nmsgpcnt-ptc nmsgpcnt-npb
nmsgpcnt-fsi:
	gcc $(CFLAGS) -Wall nmsgpcnt-fsi.c common.c -o nmsgpcnt-fsi
nmsgpcnt-ptc: 
	gcc $(CFLAGS) -Wall nmsgpcnt-ptc.c common.c src/nmsg.pb-c.c -o nmsgpcnt-ptc -lprotobuf-c
nmsgpcnt-npb: $(NMSGPCNTSRC) 
	gcc $(CFLAGS) -onmsgpcnt-npb $(NMSGPCNTSRC)
clean:
	rm -rf *.o *.dSYM nmsgpcnt-npb nmsgpcnt-fsi nmsgpcnt-ptc

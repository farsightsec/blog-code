CFLAGS += -Wall -Werror -g -O0 -Isrc/ -I.
NMSGPCNTSRC = nmsgpcnt-npb.c src/nmsg.pb.c src/pb_decode.c src/pb_common.c

all: nmsgpcnt-fsi nmsgpcnt-ptc nmsgpcnt-npb
nmsgpcnt-fsi:
	gcc $(CFLAGS) -Wall nmsgpcnt-fsi.c -o nmsgpcnt-fsi
nmsgpcnt-ptc: 
	gcc $(CFLAGS) -Wall nmsgpcnt-ptc.c src/nmsg.pb-c.c -o nmsgpcnt-ptc -lprotobuf-c
nmsgpcnt-npb: $(NMSGPCNTSRC) 
	$(CC) $(CFLAGS) -onmsgpcnt-npb $(NMSGPCNTSRC)
clean:
	rm -rf *.o nmsgpcnt-npb *.dSYM nmsgpcnt-fsi nmsgpcnt-ptc

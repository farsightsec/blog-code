CFLAGS += -Wall -Werror -g -O0

all: sratesttool
sratesttool: sratesttool.o
	gcc $(CFLAGS) sratesttool.c -o sratesttool -laxa
clean:
	rm -rf *.o *.dSYM sratesttool

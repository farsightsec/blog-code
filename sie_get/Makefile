EXEC = sie_get
PREFIX = /usr/local
MANPREFIX = /usr/local/share/man
JSON_C_DIR=/usr/local/Cellar/json-c/0.13.1

CC = clang
CFLAGS = -O3 -Wall 
CPPFLAGS = -I /usr/local/include
LDFLAGS = -L /usr/local/lib -Wall
LDLIBS = -lcurl -ljson-c

CFLAGS += -I$(JSON_C_DIR)/include/json-c
LDFLAGS+= -L$(JSON_C_DIR)/lib

.SUFFIXES:
.SUFFIXES: .c .o

objects = sie_get.o getAPIkey.o

all: $(objects)
	$(CC) -o $(EXEC) $(objects) $(LDFLAGS) $(LDLIBS)

sie_get.o :
getAPIkey.o :

.PHONY: install
install:
	mkdir -p $(PREFIX)/bin
	cp sie_get $(PREFIX)/bin/.
	chmod a+rx $(PREFIX)/bin/$(EXEC)
	mkdir -p $(MANPREFIX)/man1
	cp sie_get.1 $(MANPREFIX)/man1/.

.PHONY: clean
clean:
	@rm -f $(EXEC) $(objects)

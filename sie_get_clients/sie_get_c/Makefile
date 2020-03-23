EXEC = sie_get_c
PREFIX = /usr/local

CC = clang
CFLAGS = -Wall -Werror -std=c11 -O3 
CPPFLAGS = -I /usr/local/include 
LDFLAGS = -L /usr/local/lib -Wall 
LDLIBS = -lcurl 

.SUFFIXES:
.SUFFIXES: .c .o

objects = \
	build_filename.o \
	check_channel.o \
	check_intervals.o \
	convert_relative_times_to_real_datetimes.o \
	getkeyfromlocalfile.o \
	fix_times.o \
	isNumeric.o \
        list_channels.o \
	make_query.o \
	one_real_arg.o \
	parson.o \
        printout_intervals.o \
	print_usage_info.o \
	replString.o \
	search_a_list.o \
	sie_get_c.o \
	string_fmt_time_to_seconds.o \
	subString.o \
	three_real_args.o \
        validateapikeyonline.o \
        validate_input_time_date_format.o 

all: $(objects)
	$(CC) -o $(EXEC) $(objects) $(LDFLAGS) $(LDLIBS)

sie_get.o :
getAPIkey.o :

.PHONY: install
install:
	mkdir -p $(PREFIX)/bin
	cp $(EXEC) $(PREFIX)/bin/.
	chmod a+rx $(PREFIX)/bin/$(EXEC)
#	mkdir -p $(MANPREFIX)/man1
#	cp $(EXEC).1 $(MANPREFIX)/man1/.

.PHONY: clean
clean:
	@rm -f $(EXEC) $(objects)

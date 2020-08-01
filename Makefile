CC = gcc
MAKE = make
CFLAGS = -Og -ggdb3
RELCFLAGS = -O3 -s

all: dbg

libamdmmio/libamdmmio.a:
	$(MAKE) -f libamdmmio/Makefile

dbg: amdtempthing.c libamdmmio/libamdmmio.a
	$(CC) $(CFLAGS) amdtempthing.c libamdmmio/libamdmmio.a -o amdtempthing

release: amdtempthing.c libamdmmio/libamdmmio.a
	$(CC) $(RELCFLAGS) amdtempthing.c libamdmmio/libamdmmio.a -o amdtempthing


CC = gcc
CFLAGS = -Og -ggdb3
RELCFLAGS = -O3 -s

all: dbg

dbg: amdtempthing.c libamdmmio.a
	$(CC) $(CFLAGS) amdtempthing.c libamdmmio.a -o amdtempthing

release: amdtempthing.c libamdmmio.a
	$(CC) $(RELCFLAGS) amdtempthing.c libamdmmio.a -o amdtempthing


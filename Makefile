all:
	$(CC) $(wildcard *.c) $(CFLAGS) -shared -o libiosexec.dylib

static:
	$(CC) $(wildcard *.c) $(CFLAGS) -c
	libtool -static -o libiosexec.a $(wildcard *.o)

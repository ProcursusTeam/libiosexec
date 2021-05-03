all:
	$(CC) $(wildcard *.c) $(CFLAGS) -shared -o libiosexec.dylib

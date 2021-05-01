all:
	$(CC) $(wildcard *.c) $(CFLAGS) -shared -o libioexec.dylib

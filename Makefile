SOVER := 1

CC= xcrun --sdk iphoneos cc -arch arm64

all:
	$(CC) $(wildcard *.c) $(CFLAGS) -DLIBIOSEXEC_INTERNAL -install_name $(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/lib/libiosexec.$(SOVER).dylib -shared -o libiosexec.$(SOVER).dylib

static:
	$(CC) $(wildcard *.c) $(CFLAGS) -c
	libtool -static -o libiosexec.a *.o

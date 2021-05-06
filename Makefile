SOVER := 1

SRC := execl.c execv.c utils.c

all: libiosexec.$(SOVER).dylib libiosexec.a

libiosexec.$(SOVER).dylib: $(SRC:%.c=%.o)
	$(CC) $(CFLAGS) -DLIBIOSEXEC_INTERNAL -install_name $(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/lib/$@ -shared -o $@ $^

libiosexec.a: $(SRC:%.c=%.o)
	$(AR) cru $@ $^
	$(RANLIB) $@

install: all
	$(GINSTALL) -Dm644 libiosexec.$(SOVER).dylib $(DESTDIR)$(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/lib/libiosexec.$(SOVER).dylib
	$(LN) -sf libiosexec.$(SOVER).dylib $(DESTDIR)$(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/lib/libiosexec.dylib
	$(GINSTALL) -Dm644 libiosexec.a $(DESTDIR)$(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/lib/libiosexec.a
	$(GINSTALL) -Dm644 libiosexec.h $(DESTDIR)$(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/include/libiosexec.h

clean:
	rm -f libiosexec.$(SOVER).dylib libiosexec.a *.o

.PHONY: all clean install

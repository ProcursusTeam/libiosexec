CC       ?= cc
AR       ?= ar
LN       ?= ln
RANLIB   ?= ranlib
INSTALL  ?= install

MEMO_PREFIX     ?=
MEMO_SUB_PREFIX ?= /usr
LIBDIR          ?= $(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/lib
INCLUDEDIR      ?= $(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/include

SOVER := 1

SRC := execl.c execv.c utils.c

all: libiosexec.$(SOVER).dylib libiosexec.a

%.o: %.c
	$(CC) -c $(CFLAGS) -fvisibility=hidden -DLIBIOSEXEC_INTERNAL $^

libiosexec.$(SOVER).dylib: $(SRC:%.c=%.o)
	$(CC) $(CFLAGS) -fvisibility=hidden -DLIBIOSEXEC_INTERNAL -install_name $(LIBDIR)/$@ -shared -o $@ $^

libiosexec.a: $(SRC:%.c=%.o)
	$(AR) cru $@ $^
	$(RANLIB) $@

install: all
	$(INSTALL) -Dm644 libiosexec.$(SOVER).dylib $(DESTDIR)$(LIBDIR)/libiosexec.$(SOVER).dylib
	$(LN) -sf libiosexec.$(SOVER).dylib $(DESTDIR)$(LIBDIR)/libiosexec.dylib
	$(INSTALL) -Dm644 libiosexec.a $(DESTDIR)$(LIBDIR)/libiosexec.a
	$(INSTALL) -Dm644 libiosexec.h $(DESTDIR)$(INCLUDEDIR)/libiosexec.h

clean:
	rm -f libiosexec.$(SOVER).dylib libiosexec.a *.o

.PHONY: all clean install

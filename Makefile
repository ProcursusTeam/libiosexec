CC       ?= cc
AR       ?= ar
LN       ?= ln
RANLIB   ?= ranlib
INSTALL  ?= install

MEMO_PREFIX     ?=
MEMO_SUB_PREFIX ?= /usr
LIBDIR          ?= $(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/lib
INCLUDEDIR      ?= $(MEMO_PREFIX)$(MEMO_SUB_PREFIX)/include

DEFAULT_INTERPRETER ?= /bin/sh

SOVER := 1
SRC   := $(wildcard *.c)

ifeq ($(shell uname -s), Linux)
CFLAGS          += -fPIE -fPIC
endif

ifeq ($(LIBIOSEXEC_PREFIXED_ROOT),)
LIBIOSEXEC_PREFIXED_ROOT := 1
endif

all: libiosexec.$(SOVER).dylib libiosexec.a

%.o: %.c
	$(CC) -c $(CFLAGS) -fvisibility=hidden -D_PW_NAME_LEN=MAXLOGNAME -DLIBIOSEXEC_INTERNAL -DLIBIOSEXEC_PREFIXED_ROOT=$(LIBIOSEXEC_PREFIXED_ROOT) -DDEFAULT_INTERPRETER='"$(DEFAULT_INTERPRETER)"' $^

libiosexec.$(SOVER).dylib: $(SRC:%.c=%.o)
ifeq ($(shell uname -s), Linux)
	$(CC) $(CFLAGS) -fvisibility=hidden -DLIBIOSEXEC_INTERNAL -lbsd -shared -o $@ $^
else ifeq ($(shell uname -s), Darwin)
	$(CC) $(CFLAGS) -fvisibility=hidden -DLIBIOSEXEC_INTERNAL -install_name $(LIBDIR)/$@ -shared -o $@ $^
endif

libiosexec.a: $(SRC:%.c=%.o)
	$(AR) cru $@ $^
	$(RANLIB) $@

test: $(SRC) tests/test.c
	$(CC) $(CFLAGS) -g -D_PW_NAME_LEN=MAXLOGNAME -DLIBIOSEXEC_INTERNAL -DLIBIOSEXEC_INTERNAL -DLIBIOSEXEC_PREFIXED_ROOT=$(LIBIOSEXEC_PREFIXED_ROOT) -DDEFAULT_INTERPRETER='"$(DEFAULT_INTERPRETER)"' -DTEST -I. -o tests/test $(LDFLAGS) $^
	cd tests && ./test

install: all
	$(INSTALL) -Dm644 libiosexec.$(SOVER).dylib $(DESTDIR)$(LIBDIR)/libiosexec.$(SOVER).dylib
	$(LN) -sf libiosexec.$(SOVER).dylib $(DESTDIR)$(LIBDIR)/libiosexec.dylib
	$(INSTALL) -Dm644 libiosexec.a $(DESTDIR)$(LIBDIR)/libiosexec.a
	$(INSTALL) -Dm644 libiosexec.h $(DESTDIR)$(INCLUDEDIR)/libiosexec.h

clean:
	rm -rf libiosexec.$(SOVER).dylib libiosexec.a *.o tests/test tests/*.dSYM *.dSYM

.PHONY: all clean install

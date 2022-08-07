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

SOVER    := 1

SRC      := execl.c execv.c get_new_argv.c posix_spawn.c system.c utils.c
PWD_SRC  := getgrent.c getpwent.c pwcache.c
WRAP_SRC := fake_getgrent.c fake_getpwent.c

ifeq ($(shell uname -s), Linux)
CFLAGS          += -fPIE -fPIC
endif

ifeq ($(DEBUG),1)
CFLAGS += -g3
endif

LIBIOSEXEC_PREFIXED_ROOT ?= 0
SHEBANG_REDIRECT_PATH    ?= /

ifeq ($(LIBIOSEXEC_PREFIXED_ROOT),1)
SRC += $(PWD_SRC)
else
SRC += $(WRAP_SRC)
endif

CFLAGS += -D_PW_NAME_LEN=MAXLOGNAME -DLIBIOSEXEC_INTERNAL -DLIBIOSEXEC_PREFIXED_ROOT=$(LIBIOSEXEC_PREFIXED_ROOT) -DDEFAULT_INTERPRETER='"$(DEFAULT_INTERPRETER)"'

DEFAULT_PATH ?= /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/bin/X11:/usr/games

all: libiosexec.$(SOVER).dylib libiosexec.a

%.o: %.c libiosexec_private.h
	$(CC) -c $(CFLAGS) -fvisibility=hidden $<

libiosexec_private.h: libiosexec_private.h.in
	sed -e "s|@DEFAULT_PATH@|$(shell printf "%s\n" "$(DEFAULT_PATH)" | tr ':' '\n' | sed "p; s|^|$(SHEBANG_REDIRECT_PATH)|" | tr '\n' ':' | sed 's|:$$|\n|')|" -e "s|@SHEBANG_REDIRECT_PATH@|$(SHEBANG_REDIRECT_PATH)|" $^ > $@

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
	$(CC) $(CFLAGS) -I. -o tests/test $(LDFLAGS) $^
	cd tests && ./test

install: all
	$(INSTALL) -Dm644 libiosexec.$(SOVER).dylib $(DESTDIR)$(LIBDIR)/libiosexec.$(SOVER).dylib
	$(LN) -sf libiosexec.$(SOVER).dylib $(DESTDIR)$(LIBDIR)/libiosexec.dylib
	$(INSTALL) -Dm644 libiosexec.a $(DESTDIR)$(LIBDIR)/libiosexec.a
	$(INSTALL) -Dm644 libiosexec.h $(DESTDIR)$(INCLUDEDIR)/libiosexec.h

clean:
	rm -rf libiosexec.$(SOVER).dylib libiosexec.a *.o tests/test tests/*.dSYM *.dSYM libiosexec_private.h

.PHONY: all clean install

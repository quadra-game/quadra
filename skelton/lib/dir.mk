# Makefile pour le Universal Game Skelton
# $Id$

ifndef MAKEFILE_INCLUDED

.PHONY: local

local:
	$(MAKE) -C ..

else
ifndef RULES

TARGETS+=lib/libugs_s.a lib/libugs.so
CLEANS+=lib/libugs_s.a lib/libugs.so

ifdef SOCKS
SOCKS_EXTRA:=-lsocks5
endif

else

lib/libugs_s.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $?

lib/libugs.so: $(OBJECTS) $(SOCKS_EXTRA)
	rm -f $@
	$(LINK.cc) $(LOADLIBES) $(LDLIBS) -shared $^ -o $@

endif
endif

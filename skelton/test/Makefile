# Makefile pour le Universal Game Skelton
# $Id$

.PHONY: local

local:
	$(MAKE) -C ..

test: testmain.o -lugs
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

string: string.o -lugs
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

sound: sound.o -lugs
	$(LINK.cc) $^ $(LOADLIBES) $(LDLIBS) -o $@

dynlib: dynlib.o -lugs
	$(LINK.cc) $^ $(LOADLIBES) -ldl $(LDLIBS) -o $@


# Makefile pour le Universal Game Skelton
# $Id$

include config/target.mk

SKELTON:=skelton

CXXFLAGS+=-pipe -pedantic -Wall -Iinclude -I$(SKELTON)/include
LDFLAGS+=-L$(SKELTON)/lib

OPTFLAGS+=-m486 -O6
DEBUGFLAGS=-ggdb

ifdef RELEASE
CXX=i386-glibc20-linux-g++
CXXFLAGS+=-I/usr/i386-glibc20-linux/include/g++ $(OPTFLAGS)
LDFLAGS+=-L/usr/i386-glibc20-linux/lib -L/home/pp/lib/i386-glibc20-linux
else
CXXFLAGS+=$(DEBUGFLAGS) -D_DEBUG
endif

ifdef SOCKS
CXXFLAGS+=-DSOCKS
endif

ifeq "$(TARGET)" "linux"
CXXFLAGS+=-DUGS_LINUX -DUGS_LINUX_SVGA -DUGS_LINUX_X11
#LDLIBS+=-L/usr/X11R6/lib -lX11 -lXext -lXpm -lvga -lvgagl -lz
#ifdef RELEASE
#LDLIBS+=-ldb
#else
#LDLIBS+=-ldb1
#endif
else
# hmm, nothing else than linux is supported by this Makefile!
endif

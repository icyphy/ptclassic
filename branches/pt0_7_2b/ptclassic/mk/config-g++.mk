# Configuration makefile when g++ is used (standard version).
# $Id$
# Copyright (c) 1991 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck

# Each makefile must define ROOT, the relative position of the root
# of the directory tree, and include this file.
# ARCH is assumed set to, say, "sun4", "sun3", or "vax".

# C++ compiler to use
GNU = g++
# flags for C++ compilation
GPPFLAGS = -g
# flags for C compilation
CFLAGS = -g

# command to generate dependencies (cfront users can try CC -M)
DEPEND=g++ -MM

# where libraries are (and where to install them)
LIBDIR=$(ROOT)/lib.$(ARCH)
# where to install binaries
BINDIR=$(ROOT)/bin.$(ARCH)
# Ptolemy kernel directory
KERNDIR=$(ROOT)/src/kernel

# where the Gnu library and linker is
GNULIB=/usr/tools/gnu/lib
# linker to use for pigi and interpreter.  g++ is not used because we
# wish to control flags handed to the linker.
LINKER=$(GNULIB)/gcc-ld -e start -dc -dp
# startup module
CRT0=/lib/crt0.o
# system libraries (libraries from the environment)
SYSLIBS=-lg++ -lm $(GNULIB)/gcc-gnulib -lc
# link flags
LINKFLAGS=-S $(CRT0) -L$(LIBDIR)
# link flags if debugging symbols are to be left
LINKFLAGS_D=$(CRT0) -L$(LIBDIR)

# CONSTRUCTOR_HACK is a substitute "gnulib3" that makes g++ programs
# call constructors in reverse order.  It is not used with the
# OSF port of g++ to the Mips, or with cfront.
CONSTRUCTOR_HACK=gnulib3a.c
# CONSTRUCTOR_HACK=

# where help files go
HELPDIR=~ptolemy/lib

# root of Octtools
OCTTOOLS = $(ROOT)/octtools
# Oct libraries
OCTLIBDIR = $(OCTTOOLS)/lib.$(ARCH)



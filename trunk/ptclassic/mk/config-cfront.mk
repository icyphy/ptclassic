# Configuration makefile for Sun-4 running Cfront.
# $Id$
# Copyright (c) 1991 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
#
# This config file suffices to compile Ptolemy, except for some of
# the Thor stars.  Ptolemy requires more work to function completely
# under cfront: in particular, the DE domain and cg/gilsScheduler do
# not function correctly under cfront at present, due either to non-portable
# constructs or cfront compiler bugs.

# C++ compiler to use
GNU = /usr/lang/CC -I$(ROOT)/src/cfront
# flags for C++ compilation
GPPFLAGS = -g -Dvolatile=
# flags for C compilation
CFLAGS = -O

# command to generate dependencies
DEPEND=/usr/lang/CC -M

# where libraries are
LIBDIR=$(ROOT)/lib.CC
# where to install binaries
BINDIR=$(ROOT)/bin.CC
# Ptolemy kernel directory
KERNDIR=$(ROOT)/src/kernel

# where the Gnu library and linker is
GNULIB=
# linker to use for pigi and interpreter
LINKER=/usr/lang/CC
# startup module
CRT0=
# system libraries (libraries from the environment)
SYSLIBS=-lm
# link flags
LINKFLAGS=-L$(LIBDIR) -Bstatic
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) -Bstatic

# CONSTRUCTOR_HACK is a substitute "gnulib3" that makes g++ programs
# call constructors in reverse order.  It is not used with the
# OSF port of g++ to the Mips, or with cfront.
# CONSTRUCTOR_HACK=gnulib3a.c
CONSTRUCTOR_HACK=

# where help files go
HELPDIR=~ptolemy/lib

# root of Octtools
OCTTOOLS = $(ROOT)/octtools
# Oct libraries
OCTLIBDIR = $(OCTTOOLS)/lib.sun4


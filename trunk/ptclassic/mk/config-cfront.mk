# Configuration makefile for Sun-4 running Cfront.
# $Id$
# Copyright (c) 1991 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
#
# This config file suffices to compile Ptolemy under cfront version
# 2.1 on the Sun-4, and should be close to what is needed on other
# platforms.

# C++ compiler to use.
CPLUSPLUS = CC -I$(ROOT)/src/compat/cfront

# flags for C++ compilation.  -DPOSTFIX_OP= is needed for cfront 2.1; it
# is not needed for 3.0.
GPPFLAGS = -g $(MEMLOG) -DPOSTFIX_OP=
CFLAGS = -g
DEPEND=CC -M

# where libraries are
LIBDIR=$(ROOT)/lib.cfront
# where to install binaries
BINDIR=$(ROOT)/bin.cfront

# where the Gnu library and linker is
GNULIB=
# linker to use for pigi and interpreter
LINKER=CC
# startup module
CRT0=
# system libraries (libraries from the environment)
SYSLIBS=-lm
# link flags
LINKFLAGS=-L$(LIBDIR)
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR)

X11_INCSPEC=
X11_LIBSPEC= -lX11

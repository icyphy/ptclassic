# Configuration makefile for Sun-4 running Cfront.
# $Id$
# Copyright (c) 1991,1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck
#
# This config file suffices to compile Ptolemy under cfront version
# 2.1 on the Sun-4, and should be close to what is needed on other
# platforms.
include $(ROOT)/mk/config-default.mk

RANLIB =	ranlib

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

# If you don't strip symbols, then pigiRpc will be about 69Mb
# If you strip pigiRpc, then dynamic linking won't work
# The SunOS4.1.3 ld man page says:
# pigiRpc images produced with the -x ld flag will segv upon startup
# pigiRpc images produced with '-qoption ld -S' are still 69Mb

# link flags
LINKFLAGS=-Bstatic -L$(LIBDIR) -v
# link flags if debugging symbols are to be left
# In cfront, this will result in a 69Mb pigiRpc
LINKFLAGS_D=-Bstatic -L$(LIBDIR)

X11_INCSPEC = -I/usr/X11/include
X11_LIBSPEC = -lX11

# Ptolemy will not build the s56dsp files under cfront 
#S56DIR= $(ROOT)/vendors/s56dsp
S56DIR=

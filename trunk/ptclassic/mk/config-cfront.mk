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
# The SunOS4.1.3 ld man page says:
#     -x   Preserve only global (non-.globl) symbols in the output
#          symbol table; only enter external symbols.  This option
#          saves some space in the output file.
#
#     -X   Record local symbols,  except  for  those  whose  names
#          begin  with  L.   This  option is used by cc to discard
#          internally generated  labels  while  retaining  symbols
#          local to routines.
#
# link flags
LINKFLAGS=-L$(LIBDIR) -x
# link flags if debugging symbols are to be left
# In cfront, this will result in a 69Mb pigiRpc
LINKFLAGS_D=-L$(LIBDIR) -X

X11_INCSPEC = -I/usr/X11/include
X11_LIBSPEC = -lX11

# Ptolemy will not build the s56dsp files under cfront 
#S56DIR= $(ROOT)/vendors/s56dsp
S56DIR=

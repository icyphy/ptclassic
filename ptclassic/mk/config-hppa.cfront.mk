# Configuration makefile for an HP-PA machine (7xx or 8xx)
# running HP-UX with cfront
# $Id$
# Copyright (c) 1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  Christopher Hylands
#
#
include $(ROOT)/mk/config-default.mk

RANLIB =	ranlib
# C++ compiler to use.
CPLUSPLUS = 	CC -I$(ROOT)/src/compat/cfront

# If you turn on debugging (-g) with cfront, expect ptcl and pigiRpc to be
# about 70Mb each.
GPPDEBUGFLAGS =
CDEBUGFLAGS =

# flags for C++ compilation.  -DPOSTFIX_OP= is needed for cfront 2.1; it
# is not needed for 3.0.
GPPFLAGS =	-g -DUSG $(GPPDEBUGFLAGS) $(MEMLOG) -DPOSTFIX_OP=
CFLAGS =	-g -DUSG $(CDEBUGFLAGS) 
DEPEND =	CC -M

# where libraries are
LIBDIR =	$(ROOT)/lib.hppa.cfront
# where to install binaries
BINDIR =	$(ROOT)/bin.hppa.cfront

# where the Gnu library and linker is
GNULIB =
# linker to use for pigi and interpreter
LINKER =	CC
# startup module
CRT0 =
# system libraries (libraries from the environment)
# /usr/lib/end.o is necessary for debugging with xdb
SYSLIBS =	-lm /usr/lib/end.o

# If you don't strip symbols, then pigiRpc will be about 69Mb
# If you strip pigiRpc, then dynamic linking won't work
# The SunOS4.1.3 ld man page says:
# pigiRpc images produced with the -x ld flag will segv upon startup
# pigiRpc images produced with '-qoption ld -S' are still 69Mb
#
#      +A   Cause the linker ld(1) to use only archive libraries for all
#           occurrences of the -l option.  Also inform the C++ runtime
#           environment that no shared libraries will be used by the program.

# link flags
LINKFLAGS =	+A -L$(LIBDIR)
# link flags if debugging symbols are to be left
# In cfront, this will result in a 69Mb pigiRpc
LINKFLAGS_D =	+A -L$(LIBDIR)

X11_INCSPEC = 	-I$(ROOT)/src/compat
X11_LIBSPEC = 	-L/usr/lib/X11R5 -lX11
X11EXT_LIBSPEC = -L/usr/lib/X11R5 -lXext

# Used by xv
XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL=bsdinst

# Ptolemy will not build the s56dsp files under sun cfront or on the hppa
#S56DIR= 	$(ROOT)/vendors/s56dsp
S56DIR=

# Used to flush the cache on the hppa.  (source is in the kernel/ directory)
FLUSH_CACHE = flush_cache.o
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o

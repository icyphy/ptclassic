# Configuration makefile to make on an HP-PA machine (7xx or 8xx)
# running HP-UX with g++2.3.3.u1 or later.  The debug format is the
# one invented by Utah (HP's format is proprietary) so you will need
# gdb to debug.
#
# $Id$
# Copyright (c) 1993,1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk


RANLIB =	ranlib

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

GPPFLAGS=-DUSG -g $(MEMLOG)

# Use gcc
# Note that you cannot compile vem with gcc, you must use cc.
# Hopefully, this will go away sometime soon
CC=gcc
CFLAGS=-DUSG -g

LINKFLAGS=-L$(LIBDIR) -Xlinker -x -static
LINKFLAGS_D=-L$(LIBDIR) -g -Xlinker -x -static
X11_INCSPEC = -I$(ROOT)/src/compat
X11_LIBSPEC = -L/usr/lib/X11R5 -lX11

# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = -Wl,-a,archive

# Used by xv
XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL=bsdinst

# Used to flush the cache on the hppa.  (source is in the kernel/ directory)
FLUSH_CACHE = flush_cache.o
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o

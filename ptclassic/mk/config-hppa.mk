# Configuration makefile to make on an HP-PA machine (7xx or 8xx)
# using GNU gcc and g++
#
# $Id$
# Copyright (c) 1993,1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck, Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

#
# Programs to use
#
RANLIB =	ranlib
# Use gcc
# Note that you cannot compile vem with gcc, you must use cc.
# Hopefully, this will go away sometime soon
CC =		gcc
OPTIMIZER =	-O2
WARNINGS =	-Wall -Wcast-qual -Wcast-align
GPPFLAGS =	-DUSG -g $(MEMLOG) $(WARNINGS) $(OPTIMIZER)
CFLAGS =	-DUSG -g $(OPTIMIZER)

#
# Variables for the linker
#

# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = 	-Wl,-a,archive

LINKFLAGS = 	-L$(LIBDIR) -Xlinker -x -static
LINKFLAGS_D = 	-L$(LIBDIR) -g -Xlinker -x -static

#
# Directories to use
#
X11_INCSPEC =	-I$(ROOT)/src/compat
X11_LIBSPEC =	-L/usr/lib/X11R5 -lX11

# S56 directory is only used on sun4.
S56DIR =

#
# Variables for miscellaneous programs
#
# Used by xv
XV_RAND =	RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	bsdinst

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE

# Used to flush the cache on the hppa.  (source is in the kernel/ directory)
FLUSH_CACHE =	flush_cache.o
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o

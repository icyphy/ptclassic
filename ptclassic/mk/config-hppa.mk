# Configuration makefile to make on an HP-PA machine (7xx or 8xx)
# running HP-UX with g++2.3.3.u1 or later.  The debug format is the
# one invented by Utah (HP's format is proprietary) so you will need
# gdb to debug.
#
# $Id$
# Copyright (c) 1993 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck

# Get the g++ definitions; we override some below.
include $(ROOT)/config-g++.mk

GPPFLAGS=-DUSG -g $(MEMLOG)

# Use gcc
CC=gcc
CFLAGS=-DUSG -g

# The following is temporary until the problem with LINK_LIBGCC_SPECIAL
# is cleared up -- must pass dir with libg++.a to linker
# Note that the location of the gnu library is set to the software
# warehouse installation at Berkeley.  If your location is different
# (it probably is), modify the following line.
GNULIBDIR=/usr/sww/lib
LINKFLAGS=-L$(LIBDIR) -L$(GNULIBDIR) -Xlinker -x -static

# If the X11 include directories are in /usr/include/X11, leave
# the following symbol blank.  Otherwise define it as "-Idir" where
# dir is the PARENT of the include directory, which must end in X11.
# (this value says to use the headers we stick in with the distribution).
X11INCL = -I$(ROOT)/src/compat

# If the X11 libraries are not on the standard library search path,
# define the following symbol as "-Ldir" where dir is the directory
# containing them.  Otherwise leave it blank.
# (you may need to change this)
X11LIBDIR = -L/usr/lib/X11R5

# Tcl symbols
TCL_ROOT = $(ROOT)/tcl
TCL_H_DIR = $(TCL_ROOT)/include
TCL_LIBDIR = $(TCL_ROOT)/lib.$(ARCH)

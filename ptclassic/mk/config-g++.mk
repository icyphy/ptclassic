# Configuration makefile when g++ is used (standard version).
# $Id$
# Copyright (c) 1991 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  J. T. Buck

# Each makefile must define ROOT, the relative position of the root
# of the directory tree, and include this file.
# ARCH is assumed set to, say, "sun4", "sun3", or "vax".

# Full path to the ~ptolemy directory (you may need to change this
# for your installation)
PTOLEMY_FULL_PATH = /usr/users/ptolemy

# To enable memory logging, define the following symbol as -DMEMORYLOG.
# To disable it, define it as empty.
MEMLOG = -DMEMORYLOG

# C++ compiler to use
GNU = g++
# flags for C++ compilation
GPPFLAGS = -g $(MEMLOG)
# flags for C compilation
CFLAGS = -g

# command to use when getting files from SCCS (Gnu make)
GET=cd $(VPATH); /usr/sccs/get

# command to generate dependencies (cfront users can try CC -M)
DEPEND=g++ -MM

# where libraries are (and where to install them)
LIBDIR=$(ROOT)/lib.$(ARCH)
# where to install binaries
BINDIR=$(ROOT)/bin.$(ARCH)
# Ptolemy kernel directory
KERNDIR=$(ROOT)/src/kernel

# where the Gnu library is
GNULIB=/usr/tools/gnu/lib

# linker to use for pigi and interpreter.
LINKER=g++
# startup module
CRT0=
# system libraries (libraries from the environment)
SYSLIBS=-lg++ -lm
# link flags (tell linker to strip out debug symbols)
LINKFLAGS=-L$(LIBDIR) -Xlinker -S
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR)

# CONSTRUCTOR_HACK is a substitute "gnulib3" that makes g++1.x programs
# call constructors in reverse order.  It is not used with the
# OSF port of g++ to the Mips, or with cfront.
# CONSTRUCTOR_HACK=gnulib3a.c
CONSTRUCTOR_HACK=

# where help files go
HELPDIR=~ptolemy/lib

# root of Octtools
OCTTOOLS = $(ROOT)/octtools
# Oct libraries
OCTLIBDIR = $(OCTTOOLS)/lib.$(ARCH)

# If the X11 include directories are in /usr/include/X11, leave
# the following symbol blank.  Otherwise define it as "-Idir" where
# dir is the PARENT of the include directory, which must end in X11.
X11INCL=

# If the X11 libraries are not on the standard library search path,
# define the following symbol as "-Ldir" where dir is the directory
# containing them.  Otherwise leave it blank.
X11LIBDIR=

# LX11 is normally just -lX11, but some vendors (e.g. DEC) have put
# this library somewhere else, e.g. -lX11-mit.
LX11 = -lX11

# LXEXT is normally just -lXext, but some vendors (e.g. DEC) have put
# this library somewhere else, e.g. -lXext-mit.
LXEXT = -lXext

# Configuration makefile for an HP-PA machine (7xx or 8xx)
# running HP-UX with cfront
# $Id$
# Copyright (c) 1990-%Q% The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the
# above copyright notice and the following two paragraphs appear in all
# copies of this software.
# 
# IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
# THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
# PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
# CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
# ENHANCEMENTS, OR MODIFICATIONS.
# 
# 						PT_COPYRIGHT_VERSION_2
# 						COPYRIGHTENDKEY
#		       
# Programmers:   Sunil Samel (IMEC), Christopher Hylands
#
#
include $(ROOT)/mk/config-default.mk

#
# Programs to use
#
RANLIB =	ranlib
# C++ compiler to use.
# pigilib/POct.cc needs +a1 to initialize automatic aggregates.
CPLUSPLUS = 	CC -I$(ROOT)/src/compat/cfront +a1

# If you turn on debugging (-g) with cfront, expect ptcl and pigiRpc to be
# about 70Mb each.
GPPDEBUGFLAGS =
CDEBUGFLAGS =
# Used in octtools/vem/makefile
OCT_DEBUG_FLAGS =

# flags for C++ compilation.  -DPOSTFIX_OP= is needed for cfront 2.1; it
# is not needed for 3.0.  Apparently -DPOSTFIX_OP= is needed for HPPA CC 3.20,
# but it is unneeded for HPPA CC 3.50.  You can use 'what /usr/bin/CC'
# to find out what version of HPPA CC you are using.
GPPFLAGS =	-DUSG $(GPPDEBUGFLAGS) $(MEMLOG)
# -Aa turns on ansi c, needed for tkoct
# The cc man page says that defining _HPUX_SOURCE gives the same name space
# compatibility as -Ac
CFLAGS =	-DUSG $(CDEBUGFLAGS) -Aa -D_HPUX_SOURCE

# CC on HPs does not know the "-M" option as given in DEPEND.
# makedepend is part of X11
DEPEND =	makedepend
# common.mk look for USE_MAKEDEPEND
USE_MAKEDEPEND = 	yes

#
# Variables for the linker
#

# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = 	-Wl,-a,archive

# where the Gnu library and linker is
GNULIB =
# linker to use for pigi and interpreter
LINKER =	CC
# startup module
CRT0 =
# system libraries (libraries from the environment)
# /usr/lib/end.o is necessary for debugging with xdb
SYSLIBS =	-lm /usr/lib/end.o

# system libraries for linking .o files from C files only
CSYSLIBS = $(SYSLIBS)

# If you don't strip symbols, then pigiRpc will be about 69Mb
# If you strip pigiRpc, then dynamic linking won't work
#      +A   Cause the linker ld(1) to use only archive libraries for all
#           occurrences of the -l option.  Also inform the C++ runtime
#           environment that no shared libraries will be used by the program.
# Note that the +A option seems to cause errors about:
# c++patch: could not find __head symbol. Did you link in libcxx.a?
# We use the +A flag to produce statically linked binaries so that we can
# more easily distribute the binaries.  If you don't need statically linked
# binaries, then don't use +A.  (The chatr command will tell you about
# whether a binary is dynamically or statically linked)

# link flags
LINKFLAGS =	-L$(LIBDIR) 
# link flags if debugging symbols are to be left
# In cfront, this will result in a 69Mb pigiRpc
#   -g causes chaos with /usr/lib/end.o and xdb, so we skip it
LINKFLAGS_D =	-L$(LIBDIR)

#
# Directories to use
#
X11_INCSPEC =	-I$(ROOT)/src/compat -I/usr/sww/X11R6/include
X11_LIBSPEC =	-L/usr/sww/X11R6/lib -lX11
#X11_INCSPEC =	-I$(ROOT)/src/compat
#X11_LIBSPEC =	-L/usr/lib/X11R5 -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
X11EXT_LIBSPEC=	-L/usr/sww/X11R6/lib -lXext -lSM -lICE
#X11EXT_LIBSPEC=-lXext


#
# Variables for miscellaneous programs
#
# Used by xv
XV_RAND =	"-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	bsdinst

# Used to flush the cache on the hppa.  (source is in the kernel/ directory)
FLUSH_CACHE =	flush_cache.o
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o

# Defines to build xv
XMKMF =		/usr/sww/X11/bin/xmkmf
# -Ac turns on compatibility mode
# -DXLIB_ILLEGAL_ACCESS is needed to compile xv.c
XV_CC =		cc -Ac -DXLIB_ILLEGAL_ACCESS $(X11_INCSPEC) $(X11_LIBSPEC)

# Matlab architecture
MATARCH = hp700

# Ipus uses templates in a way that is compatible with g++, but not hppa.cfront
INCLUDE_IPUS_DOMAIN = no

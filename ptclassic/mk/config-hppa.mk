# Configuration makefile to make on an HP-PA machine (7xx or 8xx)
# using GNU gcc and g++
#
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
# Programmer:  J. T. Buck, Neal Becker, Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Note that hppa does support shl_load() style dynamic linking, see
# $(PTOLEMY)/src/kernel/Linker.sysdep.h for more information.
# You may need to get the latest HP linker patch for shl_load to work.
# As of 4/95 the linker patch was PHSS_5083*
#
# Programs to use
#
RANLIB =	ranlib
# Use gcc everywhere, including octtools
CC =		gcc
OPTIMIZER =	-O2
#-Wsynth is new in g++-2.6.x
WARNINGS =	-Wall -Wcast-qual -Wsynth
GPPFLAGS =	-DUSG -g $(MEMLOG) $(WARNINGS) $(OPTIMIZER)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =	-g -DUSG $(MEMLOG) $(WARNINGS) $(OPTIMIZER)


#
# Variables for the linker
#

# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = 	-Wl,-a,archive
# If you are trying out the shl_load feature then you want
# CC_STATIC =	

# For some reason, hppa ends up with very large (30Mb) pigiRpc binaries,
# even if we pass ld the -x option:
#   -s strips out all debugging information
#   -x is also useful, it removed local symbols, see the ld man page
# Note that -s will disable incremental linking
# As a workaround, we run strip -x after producing the pigiRpc and ptcl
# binaries.  Note that linking pigiRpc on a 32Mb hp735 can take upwards
# of 30 minutes.  GNU strip (cygnus-2.3.1 version) was reported to have 
# troubles here, so use HP strip.
STRIP_DEBUG =	/bin/strip -x
# If you are trying out the shl_load feature then you want
# STRIP_DEBUG = true

# We ship statically linked binaries, but other sites might want
# to remove the -static below
# If you are trying out the shl_load feature, then remove -static
# and add -Xlinker -E
LINKFLAGS = 	-L$(LIBDIR) -Xlinker -x -static 
LINKFLAGS_D = 	-L$(LIBDIR) -g -static


#
# Directories to use
#
#X11_INCSPEC =	-I$(ROOT)/src/compat -I/usr/sww/X11R6/include
#X11_LIBSPEC =	-L/usr/sww/X11R6/lib -lX11
X11_INCSPEC =	-I$(ROOT)/src/compat -I/usr/sww/X11R5/include
X11_LIBSPEC =	-L/usr/sww/X11R5/lib -lX11
#X11_INCSPEC =	-I$(ROOT)/src/compat
#X11_LIBSPEC =	-L/usr/lib/X11R5 -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
#X11EXT_LIBSPEC=-lXext -lSM -lICE
X11EXT_LIBSPEC=-lXext

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR = 	-collector=$(ROOT)/gnu/sun4/lib/gcc-lib/hppa1.1-hp-hpux/2.6.3/ld

# Purelink is not available on hppa
PURELINK =
PURIFY =	purelink $(COLLECTOR) purify
QUANTIFY =	purelink $(COLLECTOR) quantify
PURECOV = 	purecov $(COLLECTOR)

#
# Variables for miscellaneous programs
#
# Used by xv
# -DXLIB_ILLEGAL_ACCESS is need for X11R6 to compile xv.c:rd_str_cl()
XV_CC =		gcc -traditional $(X11_INCSPEC) $(X11_LIBSPEC) \
		-DXLIB_ILLEGAL_ACCESS
XV_RAND =	"-DNO_RANDOM -Drandom=rand"
# hppa has no isntall, bsdinst is part of X11
XV_INSTALL =	bsdinst
XMKMF =		xmkmf

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE $(X11_INCSPEC)

# Used to flush the cache on the hppa.  (source is in the kernel/ directory)
FLUSH_CACHE =	flush_cache.o
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o

# If you are trying out the shl_load feature uncomment the lines below.
#USER_CC_SHAREDFLAGS =		-fPIC
#USER_C_SHAREDFLAGS =		-fPIC
#SYSLIBS =			-lg++ -lm -ldld

# Matlab architecture
MATARCH = hp700

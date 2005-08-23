# Configuration makefile to make on an HP-PA machine (7xx or 8xx) and HPUX10.x
# using GNU gcc and g++
#
# @(#)config-hppa.mk	1.54 2/8/96
# Copyright (c) 1990-1995 The Regents of the University of California.
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

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
include $(ROOT)/mk/config-g++.shared.mk

# gcc-2.7.2 under hpux10 requires '-shared -fPIC' to produce shared
# libraries, '-shared' by itself won't work, so we override these three
# variables from config-g++.shared.mk

# Command to build C++ shared libraries
SHARED_LIBRARY_COMMAND = g++ -shared $(CC_SHAREDFLAGS) $(SHARED_COMPILERDIR_FLAG) -o

# Command to build C shared libraries
CSHARED_LIBRARY_COMMAND = gcc -shared $(C_SHAREDFLAGS) $(SHARED_COMPILERDIR_FLAG) -o

# Used by cgwork.mk
INC_LINK_FLAGS =	-shared $(CC_SHAREDFLAGS) $(SHARED_COMPILERDIR_FLAG)

# List of libraries to search, obviating the need to set SHLIB_PATH
# See the ld man page for more information.  These path names must
# be absolute pathnames, not relative pathnames.
SHARED_LIBRARY_R_LIST = -Wl,+s,+b,$(SHARED_LIBRARY_PATH)

# Note that hppa does support shl_load() style dynamic linking, see
# $(PTOLEMY)/src/kernel/Linker.sysdep.h for more information.
# You may need to get the latest HP linker patch for shl_load to work.
# As of 4/95 the linker patch was PHSS_5083* for hpux9.x
#
# Programs to use
#
RANLIB =	ranlib
# ptlang and islang use yacc, which is not necessarily part of hpux10.x
YACC =		bison -y
# islang uses lex, which is not necessarily part of hpux10.x
LEX =		flex
# src/kernel/makefile uses this to 
HP_AS =		/usr/ccs/bin/as
# Use gcc everywhere, including octtools
CC =		gcc
OPTIMIZER =	-O2
#-Wsynth is new in g++-2.6.x
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wsynth #-Wcast-qual 

# Misc. flags for OS version, if you are under HPUX9.x:
#MISCCFLAGS =	-DUSE_SHLLOAD
# If you are under HPUX10.x:
MISCCFLAGS =	-DPTHPUX10 -DUSE_SHLLOAD

# Under gcc-2.7.0, you will need to add -fno-for-scope to GPPFLAGS
GPPFLAGS =	-DUSG -g $(MEMLOG) $(WARNINGS) $(OPTIMIZER) -fno-for-scope \
		$(MISCCFLAGS)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =	-g -DUSG $(MEMLOG) $(WARNINGS) $(OPTIMIZER) $(MISCCFLAGS)


#
# Variables for the linker
#

# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
#CC_STATIC = 	-Wl,-a,archive
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
#STRIP_DEBUG =	/bin/strip -x
# If you are trying out the shl_load feature then you want
 STRIP_DEBUG = true

# We ship statically linked binaries, but other sites might want
# to remove the -static below
# If you are trying out the shl_load feature, then remove -static
# and add -Xlinker -E
#LINKFLAGS = 	-L$(LIBDIR) -Xlinker -x -static 
#LINKFLAGS_D = 	-L$(LIBDIR) -g -static
LINKFLAGS = 	-L$(LIBDIR) -Xlinker -x -Xlinker -E $(SHARED_LIBRARY_R_LIST)
LINKFLAGS_D = 	-L$(LIBDIR) -g -Xlinker -E $(SHARED_LIBRARY_R_LIST)

LIBSUFFIX =		sl

#
# Directories to use
#
# To bad hp can't ship a complete set of X includes and libs
#  in a standard location
X11DIR = 	/usr/sww/X11R5
X11_INCSPEC =	-I$(ROOT)/src/compat -I$(X11DIR)/include
X11_LIBSPEC =	-L$(X11DIR)/lib -lX11
#X11_INCSPEC =	-I$(ROOT)/src/compat
#X11_LIBSPEC =	-L/usr/lib/X11R5 -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
#X11EXT_LIBSPEC=-lXext -lSM -lICE
X11EXT_LIBSPEC=-lXext

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR = 	-collector=$(ROOT)/gnu/$(PTARCH)/lib/gcc-lib/$(PTARCH)/2.7.2/ld

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
# If you are running under HPUX9.x, you may want to 
# comment out FLUSH_CACHE and LIB_FLUSH_CACHE
FLUSH_CACHE =	flush_cache.o
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o

# If you are trying out the shl_load feature uncomment the lines below.
SYSLIBS =	-lg++ -lstdc++ -lm -ldld
## system libraries for linking .o files from C files only
CSYSLIBS = 	-lm -ldld

# Matlab architecture
MATARCH = hp700

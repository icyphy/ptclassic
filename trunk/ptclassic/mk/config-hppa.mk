# Configuration makefile to make on an HPUX9 and HPUX10 machine
# using GNU gcc and g++.  This file is setup for HPUX10, and would
# require some editing for HPUX9

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
# Programmers:  J. T. Buck, Neal Becker, Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# The HPUX9/HPUX10 dependencies are below here

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
# Under HPUX9 you will probably want to comment this out.
include $(ROOT)/mk/config-g++.shared.mk

# ptbin.mk uses this to decide whether to include the PN stars
# If you are under HPUX10, then the PN domain requires DCE threads.
#  you will need to install the DCE development set of the OS cds.
#  If you don't have a /usr/include/pthread.h, then you probably
#  don't have the DCE developement set installed.  If you don't have
#  this installed, set INCLUDE_PN_DOMAIN to no
INCLUDE_PN_DOMAIN = yes
#INCLUDE_PN_DOMAIN = 

# Misc. flags for OS version, if you are under HPUX9.x:
#ARCHFLAGS =	
# If you are under HPUX10 and don't have DCE installed, add -DPTNO_THREADS
#  to ARCHFLAGS.
# If you are under HPUX10.x:
ARCHFLAGS =	-DPTHPUX10 -D_CMA_NOWRAPPERS_ -D_REENTRANT -D_HPUX_SOURCE

# src/kernel/makefile uses this to compile flush_cache.s.
# Under HPUX9.x, use /bin/as
#HP_AS = 	/bin/as
HP_AS =		/usr/ccs/bin/as

# end of HPUX9/HPUX10 dependencies


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
# Use gcc everywhere, including octtools
CC =		gcc

# In config-$PTARCH.mk, we set the following variables.  We need to 
# use only the following variables so that we can use them elsewhere, say
# for non-optimized compiles.
# OPTIMIZER - The setting for the optimizer, usually -O2.
# MEMLOG    - Formerly used to log memory allocation and deallocation.
# WARNINGS  - Flags that print warnings.
# ARCHFLAGS - Architecture dependent flags, useful for determining which
#	      OS we are on.  Often of the form -DPTSOL2_4.
# LOCALCCFLAGS - Other architecture dependent flags that apply to all releases
#	      of the OS for this architecture for c++
# LOCALCFLAGS - Other architecture dependent flags that apply to all releases
#	      of the OS for this architecture for c++
# USERFLAGS - Ptolemy makefiles should never set this, but the user can set it.

OPTIMIZER =	-O2
#-Wsynth is new in g++-2.6.x
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wsynth #-Wcast-qual 

# Under gcc-2.7.0, you will need to add -fno-for-scope to LOCALCCFLAGS
LOCALCCFLAGS =	-g -DUSG -DUSE_SHLLOAD $(GCC_270_OPTIONS)

GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
LOCALCFLAGS =	-g -DUSG
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)



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
# and add -Wl,-E
# Under HPUX10.01, ld takes the following arguments
#   -x partially strip the output file, leaving out local symbols
#   -E export all syms
#   -G Strip all unloadable data from the output file
#   +s use SHLIB_PATH
#   +b list of directories to be searched
#LINKFLAGS = 	-L$(LIBDIR) -Xlinker -x -static 
#LINKFLAGS_D = 	-L$(LIBDIR) -g -static
LINKFLAGS = 	-L$(LIBDIR) -Wl,-x,-E,-G $(SHARED_LIBRARY_R_LIST)
LINKFLAGS_D = 	-L$(LIBDIR) -g -Wl,-E $(SHARED_LIBRARY_R_LIST)

LIBSUFFIX =		sl

#
# Directories to use
#
# To bad hp can't ship a complete set of X includes and libs
#  in a standard location
#
# If you are running HPUX10, and you plan on using the prebuilt hpux binaries
# in this directory, then you must have X11R6 installed.
# 
# If you do not, you will see messages like:
# /usr/lib/dld.sl: Unresolved symbol: XOpenIM (code)  from /users/ptolemy/bin.hppa
#
# The hpux faq says that you can download X11R6 libraries and binaries
# from http://hpux.cs.utah.edu/hppd/hpux/X11/Core/XR6built-6.11/
#
X11DIR = 	/usr/local/X11R6
X11_INCSPEC =	-I$(ROOT)/src/compat -I$(X11DIR)/include
X11_LIBSPEC =	-L$(X11DIR)/lib -lX11
# X11_LIBDIR is used in the SHARED_LIBRARY_PATH
X11_LIBDIR =	$(X11DIR)/lib

#X11_INCSPEC =	-I$(ROOT)/src/compat
#X11_LIBSPEC =	-L/usr/lib/X11R5 -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
X11EXT_LIBSPEC=-lXext -lSM -lICE
#X11EXT_LIBSPEC=-lXext

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

# HPUX has a hard time handling symbols that are defined both in a static
# library and in a shared library, so, since we have different Error::
# functions for pigiRpc, ptcl and tycho, we need to be sure that
# libpigi, libptcl and libtycho are all static on the hppa.
USE_CORE_STATIC_LIBS = 1


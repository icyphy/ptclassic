# Config file to build under NT 4 with cygnus gcc and g++
# NOTE: this is only a partial port of Ptolemy
# This file is present only as a starting point for others.
# $PTOLEMY/doc/html/nt/index.html for more information.

# @(#)config-nt4.mk	1.28 09/21/00

# Copyright (c) 1997-1999 The Regents of the University of California.
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
# Programmer:  Christopher Hylands

# This port uses Cygnus GNU-win32 b19, available from
# www.cygnus.com

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
#include $(ROOT)/mk/config-g++.shared.mk

#
# Programs to use
#
#SHELL=/Cygnus/B19/H-i386-cygwin32/bin/bash.exe
RANLIB = touch
YACC= byacc
# Use gcc everywhere including in octtools
CC =		gcc -I$(ROOT)/src/compat/nt
CPLUSPLUS =	g++ -I$(ROOT)/src/compat/cfront -DPT_EGCS -fpermissive

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

OPTIMIZER =	#-O2
# -Wsynth is a g++ flag first introduced in g++-2.6.x.
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wsynth -Wno-non-virtual-dtor #-Wcast-qual 

# PT_NO_TIMER : Used in ProfileTimer.cc and SimControl.cc
# PT_ERRNO_IS_A_FUNCTION: Used by src/octtools/internal.h
# USE_SIGACTION: Used by src/octtools/Xpackages/iv/timer.c
# HAS_TERMIOS: Used by src/octtools/Packages/iv/ivGetLine.c

# If we compile with -g, then the link can take a long time
#DEBUGCFLAG = -g 
LOCALCCFLAGS =	$(DEBUGCFLAGS) -DPTNT -DPT_NO_TIMER -DUSE_DIRENT_H \
		-DNO_SYS_SIGLIST -DPT_ERRNO_IS_A_FUNCTION $(GCC_270_OPTIONS)
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
# If you are not using gcc, then you might have problems with the WARNINGS flag
LOCALCFLAGS =	$(LOCALCCFLAGS)
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

#
# Variables for the linker
#
# system libraries (libraries from the environment) for c++ files
SYSLIBS=$(SHARED_COMPILERDIR_FLAG) $(SHARED_SYSLIBS) -lstdc++ -lm

# system libraries for linking .o files from C files only
CSYSLIBS=$(SHARED_COMPILERDIR_FLAG) -lm

# Ask ld to strip symbolic information, otherwise, expect a 32Mb pigiRpc
# -s conflicts with purelink, so if you are using purelink comment this out.
LINKSTRIPFLAGS=-Wl,-s

# Can't use -static here, or we won't be able to find -ldl, and
# dynamic linking will not work.
LINKFLAGS=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST) $(LINKSTRIPFLAGS) 
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST)

# Binaries end with this extension
BINARY_EXT = .exe

#
# Directories to use
#
X11_DIR =	/usr/X11R6.4
X11_INCSPEC =	-I$(X11_DIR)/include
X11_LIBDIR =	$(X11_DIR)/lib
X11EXT_LIBSPEC=	-lXpm -lXext -lSM -lICE
X11_LIBSPEC =	-L$(X11_LIBDIR) -lX11

# XMKMF and MKDIRHIER are used when building xv
XMKMF =		$(X11_DIR)/bin/xmkmf
MKDIRHIER =	$(X11_DIR)/bin/mkdirhier

# OCTTOOLS_ATTACHE_DIR is usually set to attache in
# $PTOLEMY/mk/config-default.mk
# but config-nt4.mk overrides it and sets it to nothing because Cygwin
# cannot compile attache because of curses issues
OCTTOOLS_ATTACHE_DIR = attache

# OCTTOOLS_IV_DIR is usually set to iv in $PTOLEMY/mk/config-default.mk
# but config-nt4.mk overrides it and sets it to nothing because Cygwin
# cannot compile iv because of signal issues.
OCTTOOLS_IV_DIR =

# Don't include Mathematica
INCLUDE_MATHEMATICA = no

# Don't includ Matlab
INCLUDE_MATLAB = no

# Don't include gthreads
INCLUDE_GTHREADS =	no

# Don't include the PN domain
INCLUDE_PN_DOMAIN =	no

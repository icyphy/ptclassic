# Makefile for FreeBSD
# Version: $Id$
#
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
#
# @(#)config-freebsd.mk
#                      
# Programmer: 	John Wehle, john@feith.com
# Modified by: 	Peter Dufault, dufault@hda.com (HD Associates, Inc)  

# Ptolemy 0.7 notes.  All first person presenatations are
# by dufault@hda.com

# If you're going to use ptolemy during the beta test use FreeBSD 2.2.1.
# This has the correct versions of the compiler,
# etc, and you won't have to install the "pgcc" port.  The 3.0 tree
# may work but of course is under test and active development.
# The older 2.1 branch will need at least the "pgcc" port of the new compiler.

# I used xv from the ports collection rather than build the one
# with Ptolemy.

# The basic process is to build the included itcl2.2 and add the shared
# libraries, build octtools, and then build ptolemy.  I provided a
# build script that worked for me.

# Note: You need a lot of swap to build Ptolemy.  I expect out of the
# box installations using the sysinstall default will not have enough.
# I have about 100MB and it just sneaks by.
# Use "vnconfig" to swap to a file if you don't have enough.

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
ifndef BUILD_STATIC_BINARIES
SHMAJOR =	7
SHMINOR =	0
include $(ROOT)/mk/config-g++.shared.mk
LIBSUFFIX =		so.$(SHMAJOR).$(SHMINOR)
LINKSHAREDFLAGS =
endif

# Command to build C++ shared libraries
SHARED_LIBRARY_COMMAND = ld /usr/lib/c++rt0.o -Bshareable -o 
 
# Command to build C shared libraries
CSHARED_LIBRARY_COMMAND =ld -Bshareable -o 

# You may want to override the "-g" in LOCALCCFLAGS for a true
# production environment - the link time and program output are large.


# You may want to override the "-g" in LOCALCCFLAGS for a true
# production environment - the link time and program output are large.

#
# Programs to use
#
RANLIB =	ranlib
# Use gcc everywhere including in octtools
CC =		gcc

CPLUSPLUS =	g++

OCTTOOLS_MM_LIB=
OCT_CC =	gcc $(OCTTOOLS_MM_LIB)

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


OPTIMIZER =	-O2 -m486 -pipe
#OPTIMIZER =	-O2 -mpentium -pipe
#OPTIMIZER =	-O2 -m486 -fomit-frame-pointer -pipe

# Under gcc-2.7.0, you will need to add -fno-for-scope to $WARNINGS
WARNINGS =	-Wall -Wcast-align $(GCC_270_OPTIONS) #-Wcast-qual 
ARCHFLAGS =	-Dfreebsd
LOCALCFLAGS =	-fwritable-strings

GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

GNULIB =	/usr/lib

#
# Variables for the linker
#

# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
CC_STATIC = 	-static

# system libraries for linking .o files from C files only
CSYSLIBS =	-lm -lc -lcompat
# system libraries (libraries from the environment)
SYSLIBS =	-lg++ -lstdc++  $(CSYSLIBS)

LINKSTRIPFLAGS=-Wl,-s
LINKFLAGS =	-L$(LIBDIR)  $(LINKSTRIPFLAGS) $(LINKSHAREDFLAGS)
LINKFLAGS_D =	-L$(LIBDIR) -g $(LINKSHAREDFLAGS)

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap

# ptcl and pigiRPc use this for cuserid.o when CGTarget is used
# only defined in this config file
COMPATLIB = 	-lcompat

#
# Directories to use
#
X11_INCSPEC = 	-I/usr/X11R6/include
X11INCL     = 	-I/usr/X11R6/include
X11_LIBSPEC = 	-L/usr/X11R6/lib -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
X11EXT_LIBSPEC=	-lXext -lSM -lICE
#X11EXT_LIBSPEC=-lXext

# Variables for Pure Inc tools (purify, purelink, quantify)
# Most users don't have Pure Inc tools, so we leave them out for release
#COLLECTOR = 	-collector=$(ROOT)/gnu/sun4/lib/gcc-lib/sun4/2.5.8/ld
#PURELINK =	purelink $(COLLECTOR)
#PURIFY =	purelink $(COLLECTOR) purify
#QUANTIFY =	purelink $(COLLECTOR) quantify

COLLECTOR =
PURELINK =
PURIFY =
QUANTIFY =

# Variable for the Ariel DSP56000 board
# Most users don't have the Ariel board, so we leave the code out
#S56DIR =	$(ROOT)/vendors/s56dsp
S56DIR =

# XV is not built for Ptolemy 0.7 on FreeBSD.  The version in the ports
# collection is used.
XV_CC =		gcc -DXLIB_ILLEGAL_ACCESS $(X11_INCSPEC) $(X11_LIBSPEC)
XMKMF =		xmkmf

# Matlab architecture.  Untested by me and taken directly from the
# Linux configuration - you'll have to have the
# linux emulator installed with the Linux matlab.
MATARCH = lnx86 #i486

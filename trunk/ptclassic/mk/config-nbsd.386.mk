# Copyright (c) 1990-1996 The Regents of the University of California.
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
# Config file to build on i386 processor (PC) running NetBSD-1.x
#
# @(#)config-nbsd.386.mk	1.9 12/15/97
#                      
# Programmer: Johnathan Reason 
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
# Use gcc everywhere including in octtools
CC =		gcc

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

# Since NetBSD supports various architectures i386 needs to be specified
# Other people might port Ptolemy to netbsd_sun, netbsd_amiga, netbsd_mac,
# etc. Use EXTRA_OPTIMIZER so optimization can be turned off in kernel
# directory becuase of a bug in /usr/lib/libg++.a that causes an undefined
# symbol in IntervalList.o. This bug should go away when libg++.a in NetBSD
# is upgraded from libg++.2.5.3 to libg++.2.5. ., or when some ports the Ptolemy
# gnu-distribution. At any rate this new varibale should have no effect on any
# of the other ports, since it is not defined in any of the other config files.
OPTIMIZER=	-m486 -pipe -O2 # -fomit-frame-pointer -pipe
ARCHFLAGS=	-Dnetbsd_i386

WARNINGS =	-Wall -Wcast-qual -Wcast-align
LOCALCCFLAGS =
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
LOCALCFLAGS =	-fwritable-strings
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

GNULIB =	/usr/lib

#
# Variables for the linker
#

# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that cc uses -Bstatic
CC_STATIC = 	-static

# system libraries for linking .o files from C files only
CSYSLIBS =	-lm

# system libraries (libraries from the environment)
# -lstdc++ needed for iostream
# -lcompat needed for cuserid
SYSLIBS=	-lg++ -lstdc++ -lcompat $(CSYSLIBS)

LINKFLAGS=-L$(LIBDIR) -Xlinker -S -Xlinker -x
LINKFLAGS_D=-L$(LIBDIR) -g 

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap

# ptcl and pigiRPc use this for cuserid.o when CGTarget is used
# only defined in this config file
COMPATLIB = -lcompat

#
# Directories to use
#
X11_INCSPEC = -I/usr/X11R6/include
X11INCL     = -I/usr/X11R6/include
X11_LIBSPEC = -L/usr/X11R6/lib -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
X11EXT_LIBSPEC=-lXext -lSM -lICE
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

# Used to compile xv.  Use -traditional to avoid varargs problems
XV_CC =		gcc -traditional

# Unknown Matlab architecture - No NetBSD port of Matlab as of 5/27/95

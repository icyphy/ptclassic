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
# Config file to build on i386 processor (PC) running NetBSD-1.x
#
# $Id$
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
OCT_CC =	gcc -traditional $(OCTTOOLS_MM_LIB)

# Since NetBSD supports various architectures i386 needs to be specified
# Other people might port Ptolemy to netbsd_sun, netbsd_amiga, netbsd_mac,
# etc. Use EXTRA_OPTIMIZER so optimization can be turned off in kernel
# directory becuase of a bug in /usr/lib/libg++.a that causes an undefined
# symbol in IntervalList.o. This bug should go away when libg++.a in NetBSD
# is upgraded from libg++.2.5.3 to libg++.2.5. ., or when some ports the Ptolemy
# gnu-distribution. At any rate this new varibale should have no effect on any
# of the other ports, since it is not defined in any of the other config files.
SYSTEMDEF=	-Dnetbsd_i386
OPTIMIZER=	-m486 -pipe
EXTRA_OPTIMIZER=-O2
#OPTIMIZER=	-O2 -m486 -fomit-frame-pointer -pipe

WARNINGS =	-Wall -Wcast-qual -Wcast-align
GPPFLAGS =	$(SYSTEMDEF) $(WARNINGS) $(EXTRA_OPTIMIZER) $(OPTIMIZER) $(MEMLOG)
CFLAGS =	$(SYSTEMDEF) $(WARNINGS) $(EXTRA_OPTIMIZER) $(OPTIMIZER) $(MEMLOG) \
		-fwritable-strings
GNULIB =	/usr/lib

#
# Variables for the linker
#

# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that cc uses -Bstatic
CC_STATIC = 	-static

SYSLIBS=-lg++ -lm

LINKFLAGS=-L$(LIBDIR) -Xlinker -S -Xlinker -x -static
LINKFLAGS_D=-L$(LIBDIR) -g -static

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap

# ptcl and pigiRPc use this for cuserid.o when CGTarget is used
# only defined in this config file
COMPATLIB = -lcompat

#
# Directories to use
#
X11_INCSPEC = -I/usr/X11/include
X11INCL     = -I/usr/X11/include
X11_LIBSPEC = -L/usr/X11/lib -lX11

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

# Variables for local Matlab installation
# -- If Matlab is installed, then MATLABDIR points to where MATLAB is installed
#    and MATLABLIBDIR points to the directory containing the Matlab libraries
# -- If Matlab is not installed, then MATLABDIR equals $ROOT/src/compat/matlab
#    and MATLABLIBIDR is undefined
#MATLABDIR =	/usr/sww/matlab
#MATLABLIBDIR =	-L$(MATLABDIR)/extern/lib/$(ARCH)
MATLABDIR =	$(ROOT)/src/compat/matlab
MATLABLIBDIR =


# Used to compile xv.  Use -traditional to avoid varargs problems
XV_CC =		gcc -traditional

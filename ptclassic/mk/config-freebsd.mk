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
# Programmer: John Wehle 
#             john@feith.com
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
CPLUSPLUS =	g++ -I$(ROOT)/src/compat/freebsd

OCTTOOLS_MM_LIB=
OCT_CC =	gcc $(OCTTOOLS_MM_LIB)

SYSTEMDEF=	-Dfreebsd
OPTIMIZER=	-O2 -m486 -pipe
#OPTIMIZER=	-O2 -m486 -fomit-frame-pointer -pipe

# Under gcc-2.7.0, you will need to add -fno-for-scope to $WARNINGS
WARNINGS =	-Wall -Wcast-align #-Wcast-qual 
GPPFLAGS =	$(SYSTEMDEF) $(WARNINGS) $(EXTRA_OPTIMIZER) $(OPTIMIZER) $(MEMLOG)
CFLAGS =	$(SYSTEMDEF) $(WARNINGS) $(EXTRA_OPTIMIZER) $(OPTIMIZER) $(MEMLOG) \
		-fwritable-strings
GNULIB =	/usr/lib

#
# Variables for the linker
#

# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
CC_STATIC = 	-static

SYSLIBS=-lg++ -lm -lc -lcompat

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
XV_CC =		gcc -traditional $(X11_INCSPEC) $(X11_LIBSPEC)
XMKMF =		xmkmf

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
#

# In February  1996, the following configuration was tested:
# OS: FreeBSD 2.1-STABLE.  This should work OK with 2.1-Release and
# 2.1-Stable.  Any of the post 2.1-Stable snapshots are good bets since
# people will be real interested in problems.  2.2 users are on their
# own, as they know.
# 
# Compiler: the Pentium FreeBSD port of gcc 2.7.2 in the FreeBSD ports
# collection with the following minor fix to disable .weak symbols.
# This patch will be installed in the port, so hopefully no one else
# will need to apply it.  I built ptolemy with both "-m486" and
# "-mpentium", and it appears to work OK in both cases.  I didn't do any
# timing studies.  Here is the .weak patch for the compiler:
# 
# [The file to modify would be $PTOLEMY/src/gnu/src/gcc/config/i386/freebsd.h
# This patch fixes a bug where ld gets errors because there are multiply
# defined symbols in different .o files that make up a library. ]
#
# *** freebsd.h.b4	Mon Feb 12 18:40:53 1996
# --- freebsd.h	Mon Feb 12 18:41:25 1996
# ***************
# *** 122,130 ****
# --- 122,134 ----
#   
#   /* This is how we tell the assembler that a symbol is weak.  */
#   
# + #if 0
# + /* This requires support in both the assembler and the loader:
# +  */
#   #define ASM_WEAKEN_LABEL(FILE,NAME) \
#     do { fputs ("\t.weak\t", FILE); assemble_name (FILE, NAME); \
#          fputc ('\n', FILE); } while (0)
# + #endif
#   
#   /* The following macro defines the format used to output the second
#      operand of the .type assembler directive.  Different svr4 assemblers
# 
# Library: libg++-2.7.1 directly from prep.  It configures and installs with
# no problems.
#
# Make: gmake 3.74 from the FreeBSD ports collection, installed in the
# search path as make.  Hopefully next time around we'll set it up
# so "gmake MAKE=gmake" will work properly.
#  
# Note that Ptolemy0.6 uses itcl2.0 for the tycho editor and the gantt chart
# viewer, so you will probably want to install itcl from the 
# Ptolemy other.src tar file

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

LINKFLAGS =	-L$(LIBDIR) -Xlinker -S -Xlinker -x -static
#LINKFLAGS =	-L$(LIBDIR) -Xlinker -S -Xlinker -x
LINKFLAGS_D =	-L$(LIBDIR) -g -static

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

# Used to compile xv.  Use -traditional to avoid varargs problems
# I'm not proud of this "const" work around for FreeBSD.  I'm not sure
# why NetBSD doesn't need it - We need "-traditional" for varargs and
# then we get into problems with "const" in the headers.
XV_CC =		gcc -Dconst= -D__const= -traditional \
			-DXLIB_ILLEGAL_ACCESS $(X11_INCSPEC) $(X11_LIBSPEC)
XMKMF =		xmkmf

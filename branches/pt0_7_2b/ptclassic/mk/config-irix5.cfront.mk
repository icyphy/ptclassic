# Configuration makefile to build on SGI Indigo running Irix5.3 with
# SGI's C++ compiler (This compiler probably is not cfront, but we call 
# all non-g++ compilers 'cfront' for historical reasons)
#
# $Id$
# Copyright (c) 1994-%Q% The Regents of the University of California.
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
#	Programmers: Dan Ellis (MIT Media Lab), Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
##include $(ROOT)/mk/config-g++.mk
include $(ROOT)/mk/config-cfront.mk

#
# Programs to use
#
# C++ compiler to use
GPPFLAGS =	$(GPPDEBUGFLAGS) $(MEMLOG)
# C compiler flags.  Defined by the arch-config mk.
CFLAGS =

# IRIX5.x does not have a ranlib
RANLIB = 	true
CC = cc
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC = cc -D_BSD_SIGNALS
# BSD_SIGNALS needed *only* for src/octtools/Xpackages/iv/timer.c, 
# but put it here for yuks

# command to generate dependencies (cfront users can try CC -M)
##DEPEND=g++ -MM
DEPEND=CC -M

# linker to use for pigi and interpreter.
LINKER=CC
# system libraries (libraries from the environment)
SYSLIBS=-lm

OPTIMIZER =
# -g
# debug info uses too much disk space
##WARNINGS =	-Wall -Wcast-qual
WARNINGS  =

# Use -D_BSD_SIGNALS for src/kernel/SimControl.cc
#  see /usr/include/sys/signals.h for more info.
# Use -D_BSD_TIME for src/kernel/Clock.cc, see /usr/include/sys/time.h
MISC_DEFINES =	-D_BSD_SIGNALS -D_BSD_TIME


# dpwe: -G 0 is to disable accessing data off 'the global pointer' 
# (according to man CC); this is known to result in global space 
# overflow in large programs;  However, man CC also suggests that 
# unless -nonshared is also specified, it is ignored anyway.
#GOTFLAG = -G 0
# -xgot is the SGI hack to avoid overflows in the GOT by allowing 
# 32 bit offsets, or something.
GOTFLAG = -G 0 -xgot

#  Under gcc-2.5.8 on Irix5.2, -g is not supported
GPPFLAGS =	$(GOTFLAG) $(MEMLOG) $(WARNINGS) $(MISC_DEFINES) $(OPTIMIZER)

#     -cckr   The traditional K&R/Version7 C with SGI extensions
# Note that -cckr will not work with gcc
CFLAGS =	$(GOTFLAG) -cckr $(OPTIMIZER)
##CFLAGS =	$(GOTFLAG) $(MEMLOG) $(WARNINGS) $(MISC_DEFINES) $(OPTIMIZER)
#
# Variables for the linker
#
SYSLIBS =	-lm -lmld

# -s strips out debugging information, otherwise we get a 30Mb pigiRpc
# Unfortunately, -s causes the dreaded "Fatal error in c++patch" under 5.3 
# .. omit for now.
# -x is also useful, it removed local symbols, see the ld man page
# Unfortunately, linking -x on SGI's DCC generates executables that 
# just dump core.  So we'll have to settle for huge links and then 
# run 'strip' on the executables by hand later.
##LINKFLAGS =	-L$(LIBDIR) $(GOTFLAG) -Xlinker -s
LINKFLAGS =	-L$(LIBDIR) $(GOTFLAG)
##LINKFLAGS_D =	-L$(LIBDIR) $(GOTFLAG)
LINKFLAGS_D =	-L$(LIBDIR) $(GOTFLAG) 

#
# Directories to use
#
##X11_INCSPEC = -I/usr/X11/include
##X11_LIBSPEC = -L/usr/X11/lib -lX11

# S56 directory is only used on sun4.
S56DIR =

# Variables for local Matlab installation
# -- If Matlab is installed, then MATLABDIR points to where MATLAB is installed
#    and MATLABLIBDIR points to the directory containing the Matlab libraries
# -- If Matlab is not installed, then MATLABDIR equals $ROOT/src/compat/matlab
#    and MATLABLIBIDR is undefined
#MATLABDIR =	/usr/local/matlab
#MATLABLIBDIR =	-L$(MATLABDIR)/extern/lib/$(ARCH)
MATLABDIR =	$(ROOT)/src/compat/matlab
MATLABLIBDIR =

#
# Variables for miscellaneous programs
#
# Used by xv
#XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	/usr/bin/X11/bsdinst
XV_CC =		cc -cckr -DSVR4 -DXLIB_ILLEGAL_ACCESS

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE


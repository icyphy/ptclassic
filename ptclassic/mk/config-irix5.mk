# Configuration makefile to build on SGI Indigo running Irix5.x
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
#	Programmer: Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

#
# Programs to use
#
# IRIX5.x does not have a ranlib
RANLIB = 	true
CC =		gcc
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC =	gcc -fwritable-strings

OPTIMIZER =	-O2
#-Wsynth is new in g++-2.6.x
WARNINGS =	-Wall -Wcast-qual -Wsynth

# Use -D_BSD_SIGNALS for src/kernel/SimControl.cc
#  see /usr/include/sys/signals.h for more info.
# Use -D_BSD_TIME for src/kernel/Clock.cc, see /usr/include/sys/time.h
MISC_DEFINES =	-D_BSD_SIGNALS -D_BSD_TIME

#  Under gcc-2.5.8 on Irix5.2, -g is not supported
GPPFLAGS =	-G 0 $(MEMLOG) $(WARNINGS) $(MISC_DEFINES) $(OPTIMIZER)

#     -cckr  The traditional K&R/Version7 C with SGI extensions
# Note that -cckr will not work with gcc
#CFLAGS =	-G 0 -cckr $(OPTIMIZER)
CFLAGS =	-G 0 $(MEMLOG) $(WARNINGS) $(MISC_DEFINES) $(OPTIMIZER)
#
# Variables for the linker
#
SYSLIBS =	-lg++ -lm -lmld

# -s strips out debugging information, otherwise we get a 30Mb pigiRpc
# -x is also useful, it removed local symbols, see the ld man page
LINKFLAGS =	-L$(LIBDIR) -G 0 -Xlinker -s
LINKFLAGS_D =	-L$(LIBDIR) -G 0

#
# Directories to use
#
X11_INCSPEC = -I/usr/X11/include
X11_LIBSPEC = -L/usr/X11/lib -lX11

#
# Variables for miscellaneous programs
#
# Used by xv
#XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	/usr/bin/X11/bsdinst
XV_CC =		cc -cckr -DSVR4 -DXLIB_ILLEGAL_ACCESS

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE


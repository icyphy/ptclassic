# Configuration makefile to build on SGI Indigo running Irix5.x
#
# $Id$
# Copyright (c) 1994 The Regents of the University of California.
#                       All Rights Reserved.
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

# S56 directory is only used on sun4.
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

#
# Variables for miscellaneous programs
#
# Used by xv
#XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	/usr/bin/X11/bsdinst
XV_CC =		cc -cckr -DSVR4 -DXLIB_ILLEGAL_ACCESS

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE

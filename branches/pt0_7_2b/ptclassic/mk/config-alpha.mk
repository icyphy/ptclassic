#
# Config file to build on an Alpha AXP running DEC OSF/1,
# 
# Derived from config file for mips processor (DECstation) running Ultrix 4.x
#
# $Id$
# Copyright (c) 1991-1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Author:  Kevin D. Kissell <kkissell@acri.fr>
#

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
CC =		gcc

# At A.C.R.I. RCS is used instead of SCCS
# command to use when getting files from source control (Gnu make)
#GET=co

# If we have g++, then compile Octtools with gcc.  ARCHs that are cfront
# based probably don't have gcc.
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
# On the mips with Ultrix4.3A, don't use -traditional, or you will get
# errors sys/types.h
OCT_CC =	gcc -fwritable-strings

OPTIMIZER =	-O2
WARNINGS =	-Wall -Wcast-qual
GPPFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER)

#
# Variables for the linker
#
LINKFLAGS =	-L$(LIBDIR) -Xlinker -x
LINKFLAGS_D =	-L$(LIBDIR)

#
# Directories to use
#
#X11_INCSPEC =	-I$(ROOT)/src/compat
X11_LIBSPEC =	-L/usr/X11/lib -lX11

# Variable for the Ariel DSP56000 board--- only used on sun4.
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

############################################################################
# Debugging Hacks:  These really should not be necessary!

# We're not getting tk/tcl from the Ptolemy tree, it's already installed.
TK_LIBSPEC =	-L$(TCL_ROOT)/tk.$(ARCH)/lib -ltk

# The dynamically linked version of pigiRpc has problems that *may*
# stem from a symbol collision (clog) between libm and libg++.
LINKFLAGS =	-static $(LINKFLAGS_D) 

# For some silly reason, the X11 library has a function linked in
# which wants to see DECnet.  Fortunately, science has a cure.
X11_LIBSPEC =	-L/usr/X11/lib -lX11 -ldnet_stub
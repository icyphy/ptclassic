#
# Config file for Unixware1.1

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
#
# Config file to build on Unixware 1.1
#
# %Z% Your SCCS code here!

# Author: Steven Kimball, skimball@sanders.com

# You may find in helpful to have the text below in your .profile, or
# the equivalent in your .cshrc if you use csh.
#---start---
#ARCH=unixware
#export ARCH
#
#PTOLEMY=/home/ptolemy
#export PTOLEMY
#
#PATH=/usr/X/bin:$PTOLEMY/bin.$ARCH:$PTOLEMY/bin:/tools/gccs/bin:/tools/gnu/bin:$PATH
#export PATH
#
#LD_LIBRARY_PATH=$PTOLEMY/lib.$ARCH:/usr/X/lib
#export LD_LIBRARY_PATH
#---end---

# Note that if you are using gcc-2.7.0, you will need a patch, see the
# ptolemy-hackers archives, or 
# ftp://ptolemy.eecs.berkeley.edu/pub/ptolemy/ptolemy0.5.2/patches/README


#
#
HAVE_ELF=yes
USE_ELF=yes

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

include $(ROOT)/mk/config-g++.mk

#
# Programs to use
#
RANLIB = 	true
CC =		gcc
OCT_CC =	gcc
CPLUSPLUS =	g++
LD	=	ld
CPP	=	gcc -E
OBJDUMP	=	objdump
OBJDUMP_FLAGS =	-k -q 
LDFLAGS	=	
AS      =       as

LINKER	=	$(CPLUSPLUS)
CLINKER	=	$(CC)
RANLIB	= 	true

# common.mk looks at this variable to decide how to build shared libraries
# put this in the make command when you build ptolemy: USE_SHARED_LIBS=yes
#
# Note: If you are using gas-2.5.2 (which you must do if you want to
# debug C++ code by using stabs debug format) you CANNOT use shared
# libraries (gas does NOT support -fPIC).  If you want to use shared libraries
# then make sure you are not using gas (in my case I'd move /tools/gccs/bin/as 
# to /tools/gccs/bin/gas) and compile with OPTIMIZER=-O (or anything else, 
# just NO -g) So at the top level you'd do:
# make clean
# make OPTIMIZER=-O USE_SHARED_LIBS=yes install
#
# You need to use the same flags on the stars you compile for dynamic linking.
#
ifeq ($(USE_SHARED_LIBS), yes)
# Command to build shared libraries (Not really supported yet)
SHARED_LIBRARY_COMMAND =	ld -G -z text -o

# Use Position Independent Code to build shared libraries
C_SHAREDFLAGS =	-fPIC
CC_SHAREDFLAGS = -fPIC
# mk/userstars.mk uses these vars
USER_C_SHAREDFLAGS =	$(C_SHAREDFLAGS)
USER_CC_SHAREDFLAGS =	$(CC_SHAREDFLAGS)
LIBSUFFIX =		so
endif

# debug version
UWDEF =	-DPTSVR4
#	Don't use -pipe, it makes life worse on small-memory systems.
#	Don't use -m486, it's the default, except for those with the
#	Pentium optimized compiler; for them -m486 makes things worse.
OPTIMIZER =	-g

#
# 'production' version with optimization
#OPTIMIZER =	-O2 #-fomit-frame-pointer #-m486 -pipe

# -Wsynth is new in g++-2.6.x
WARNINGS =	-Wall -Wcast-align -Wsynth
GPPFLAGS =	$(UWDEF) $(WARNINGS) $(OPTIMIZER) $(MEMLOG) -fno-for-scope
CFLAGS =	$(UWDEF) $(OPTIMIZER) -fwritable-strings

#
# Variables for the linker
#
# system libraries (libraries from the environment)
# use gnu -liberty for strcasecmp() only (I think).
# the -lg++ is added by g++ link command, -lcurses is for g++ shared lib
ifeq ($(USE_SHARED_LIBS), yes)
SYSLIBS=-lsocket -lnsl -ldl -lm -liberty -lg++ -lcurses
else
SYSLIBS=-lsocket -lnsl -ldl -lm -liberty -lg++ -lcurses
endif

PTLANGLIB= -L/tools/gccs/lib -liberty

LINKFLAGS=-L$(LIBDIR)
LINKFLAGS_D=-g -L$(LIBDIR)

# Ask ld to strip symbolic information, otherwise, expect a 32Mb pigiRpc
LINKSTRIPFLAGS=

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap

#
# Directories to use
#
X11_INCSPEC = -I/usr/X/include
X11INCL     = -I/usr/X/include
X11_LIBSPEC = -L/usr/X/lib -lX11 -lnsl

# Flag that cc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = #-static


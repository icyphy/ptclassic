#
# Config file to build on sun4 processor (SparcStation) running Solaris2.4
# with gcc and g++

# @(#)config-sol2.mk	1.25 8/7/95

# Copyright (c) 1990-1995 The Regents of the University of California.
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
# Programmer:  Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
#include $(ROOT)/mk/config-g++.shared.mk

#
# Programs to use
#
RANLIB = true
# Use gcc everywhere including in octtools
CC =		gcc

OPTIMIZER =	-O2
# -Wsynth is new in g++-2.6.x
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wsynth #-Wcast-qual 
MULTITHREAD =	-D_REENTRANT
# Define PTSOL2_4 if you are on Solaris2_4
# Under gcc-2.7.0, you will need -fno-for-scope for GPPFLAGS
LOCALFLAGS =	-DPTSOL2_4 -pipe -fno-for-scope
GPPFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER) $(MULTITHREAD) $(LOCALFLAGS)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER) $(MULTITHREAD) $(LOCALFLAGS)

#
# Variables for the linker
#
# system libraries (libraries from the environment)
SYSLIBS=$(SHARED_COMPILERDIR_FLAG) -lsocket -lnsl -ldl -lg++ $(SHARED_SYSLIBS) -lm

# Ask ld to strip symbolic information, otherwise, expect a 32Mb pigiRpc
# -s conflicts with purelink, so if you are using purelink comment this out.
#LINKSTRIPFLAGS=-Wl,-s

# Can't use -static here, or we won't be able to find -ldl, and
# dynamic linking will not work.
LINKFLAGS=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST) $(LINKSTRIPFLAGS) 
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST)

# These are the additional flags that we need when we are compiling code
# which is to be dynamically linked into Ptolemy
INC_LINK_FLAGS = -fpic


# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that cc uses -Bstatic
CC_STATIC =

#
# Directories to use
#
X11_INCSPEC =	-I/usr/openwin/include
X11_LIBDIR =	/usr/openwin/lib
X11_LIBSPEC =	-L$(X11_LIBDIR)  -lX11

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR =

PURELINK =	purelink $(COLLECTOR) -hardlink=yes
PURIFY =	purelink $(COLLECTOR) purify
QUANTIFY =	purelink $(COLLECTOR) quantify
PURECOV = 	purecov $(COLLECTOR)

#
# Variables for miscellaneous programs
#
# Used by xv
#XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =

# -DATT is needed so we don't try and include sys/dir.h
# -R$(X11LIB_DIR) is need so we can find the X libs at runtime,
#	otherwise, we will need to set LD_LIBRARY_PATH
XV_CC =		gcc -traditional $(X11_INCSPEC) \
		-DSVR4 -DSYSV -DDIRENT -DATT -DNO_BCOPY \
		$(X11_LIBSPEC) -R$(X11_LIBDIR)

XV_RAND = 	-DNO_RANDOM

# Under sol2, xmkmf is not compatible with gcc, so we don't use it
XMKMF =		rm -f Makefile; cp Makefile.std Makefile

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE $(X11_INCSPEC)

# Matlab architecture
MATARCH = sol2

# Config file to build under NT 4 with cygnus gcc and g++
# NOTE: PTOLEMY WILL NOT COMPILE UNDER NT
# This file is present only as a starting point for others.
# See http://ptolemy.eecs.berkeley.edu/~cxh/nt-ptolemy.html

# $Id$

# Copyright (c) 1997 The Regents of the University of California.
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

# Under NT, build a static ptcl first

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
#include $(ROOT)/mk/config-g++.shared.mk

#
# Programs to use
#
RANLIB = true
# Use gcc everywhere including in octtools
CC =		gcc

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

OPTIMIZER =	#-O2
# -Wsynth is new in g++-2.6.x
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wsynth #-Wcast-qual 
LOCALCCFLAGS =	-g -DPTNT -DPT_NO_TIMER $(GCC_270_OPTIONS)
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
# If you are not using gcc, then you might have problems with the WARNINGS flag
LOCALCFLAGS =	$(LOCALCCFLAGS)
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

TCL_VERSION_NUM=76i
TK_VERSION_NUM=42i
ITCL_VERSION_NUM=22

# Combined -L and -l options to link with tcl library.
TCL_LIBSPEC= c:/Itcl2.2/tcl7.6/win/Tcl76i.lib
#TCL_LIBSPEC=-L$(TCL_ROOT)/Bin -ltcl$(TCL_VERSION_NUM)

# Directory containing Tk include files
TK_INCDIR=$(TCL_ROOT)/itk/include

# Combined -L and -l options to link with tk library.  Can add
# addtional -L and/or -l options to support tk extensions.
TK_LIBSPEC=-L$(TCL_ROOT)/itcl.$(PTARCH)/lib/itcl -ltk$(TK_VERSION_NUM) #-lXpm

# Directory containing itcl include files
ITCL_INCDIR=$(TCL_ROOT)/$(ITCL_VERSION)/include
ITCL_LIBSPEC= c:/Itcl2.2/itcl/win/Itcl22.lib 
#
# Variables for the linker
#
# system libraries (libraries from the environment) for c++ files
SYSLIBS=$(SHARED_COMPILERDIR_FLAG) -lg++ $(SHARED_SYSLIBS) -lm

# system libraries for linking .o files from C files only
CSYSLIBS=$(SHARED_COMPILERDIR_FLAG) -lm

# Ask ld to strip symbolic information, otherwise, expect a 32Mb pigiRpc
# -s conflicts with purelink, so if you are using purelink comment this out.
LINKSTRIPFLAGS=-Wl,-s

# Can't use -static here, or we won't be able to find -ldl, and
# dynamic linking will not work.
LINKFLAGS=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST) $(LINKSTRIPFLAGS) 
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST)

# These are the additional flags that we need when we are compiling code
# which is to be dynamically linked into Ptolemy.  -shared is necessary
# with gcc-2.7.0
INC_LINK_FLAGS = -shared $(SHARED_COMPILERDIR_FLAG)


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

PURELINK =	#purelink $(COLLECTOR) -hardlink=yes
PURIFY =	#purify -automount-prefix=/tmp_mnt:/vol -best-effort
QUANTIFY =	#quantify


PURECOV = 	#purecov

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
XPM_DEFINES =	#-DZPIPE $(X11_INCSPEC)

# Matlab architecture
MATARCH = unknown

# Mathematica MathLink library
MATHLINKLIBNAME = MLelf

# Build gthreads
INCLUDE_GTHREADS =	yes

# Include the PN domain.
INCLUDE_PN_DOMAIN =	yes


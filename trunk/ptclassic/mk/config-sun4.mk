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
# Config file to build on sun4 processor (SparcStation) running SunOS4.1x
#
# $Id$

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

OPTIMIZER =	-O2
# -Wsynth is new in g++-2.6.x
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wsynth #-Wcast-qual 
# Under gcc-2.7.0, you will need -fno-for-scope for LOCALCCFLAGS
LOCALCCFLAGS =	-fno-for-scope
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

#
# Variables for the linker
#

# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that cc uses -Bstatic
CC_STATIC = 	-static

# system libraries for linking .o files from C files only
CSYSLIBS =	-lm

# system libraries (libraries from the environment) for c++ files
# gcc-2.7.0/libg++-2.7 seems to require -liostream and -liberty (for memmove)
SYSLIBS =	-lg++ -lstdc++ $(CSYSLIBS)


#
# Directories to use
#
X11_INCSPEC =	-I/usr/X11/include
X11_LIBSPEC =	-L/usr/X11/lib -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
X11EXT_LIBSPEC=-lXext -lSM -lICE
#X11EXT_LIBSPEC=-lXext

# Variables for Pure Inc tools (purify, purelink, quantify)
#COLLECTOR = 	-collector=$(ROOT)/gnu/$(PTARCH)/lib/gcc-lib/$(PTARCH)/2.7.2/ld
COLLECTOR = 	-collector=$(ROOT)/gnu/$(PTARCH)/lib/gcc-lib/$(PTARCH)/2.7.2/ld-2.6.3

PURELINK =	purelink $(COLLECTOR) -hardlink=yes
PURIFY =	purelink $(COLLECTOR) purify
QUANTIFY =	purelink $(COLLECTOR) quantify
PURECOV = 	purecov $(COLLECTOR)

# Used to compile xv.  Use -traditional to avoid varargs problems
XV_CC =		gcc -traditional -DXLIB_ILLEGAL_ACCESS

# Matlab architecture
MATARCH = sun4

# Build gthreads
INCLUDE_GTHREADS =	yes

# Include the PN domain.
INCLUDE_PN_DOMAIN =	yes

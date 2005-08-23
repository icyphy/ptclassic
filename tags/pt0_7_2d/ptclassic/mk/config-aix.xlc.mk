# Config file to build on IBM RS6000 computers running AIX 3.2 with
# IBM's xlc and xlC compilers

# Copyright (c) 1990-1996 The Regents of the University of California.
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
# @(#)config-aix.xlc.mk	1.8 07/03/97
#
#	Programmer: Alexander Kurpiers
#		    <kurpiers@zeus.uet.e-technik.th-darmstadt.de>
#
# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk
# Get the g++ definitions; we override some below.
#include $(ROOT)/mk/config-g++.mk
CC = xlC
include $(ROOT)/mk/config-cfront.mk
#
# Programs to use
#
RANLIB =	ranlib
DEPEND = xlC -M -+ -Daix -Daix_xlC -I$(ROOT)/src/compat/cfront
# C++ compiler to use.
CPLUSPLUS =     xlC -+ -I$(ROOT)/src/compat/cfront


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
#WARNINGS =	-Wall -Wcast-qual
WARNINGS =
LOCALCCFLAGS =	 -qlanglvl=compat -Daix -Daix_xlC
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
LOCALCFLAGS =	-Daix -Daix_xlC
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

SYSLIBS =	-L/usr/lpp/xlC/lib -lC -lm -lc -lbsd

# system libraries for linking .o files from C files only
CSYSLIBS = 	$(SYSLIBS)

#
# Variables for the linker
#
LINKER =	xlC 
LINKFLAGS =	-L$(LIBDIR) -bnso -bI:/lib/syscalls.exp
LINKFLAGS_D =	-L$(LIBDIR) -bnso -bI:/lib/syscalls.exp
# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that cc uses -Bstatic
CC_STATIC = 	-bnso

#
# Directories to use
#
X11_INCSPEC =	-I/usr/include/X11
SMT_SYSCALLS =  -bI:/lib/syscalls.exp -bI:/usr/lpp/X11/bin/smt.exp
X11_LIBSPEC =	$(SMT_SYSCALLS) -L/usr/lib/X11 -lX11 -lXt -lXm -liconv -lpthreads
#X11_LIBSPEC =  -L/usr/lpp/X11/lib -lX11 -lXt -lXm -liconv
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
XV_CC =		cc -traditional

# for Octools try without -traditional
# use GCC because it was easier to port
OCT_CC =        gcc  -fwritable-strings
TERMLIB_LIBSPEC = -ltermcap

# Matlab architecture
MATARCH =	aix

# Mathematica architecture
MATHEMATICAARCH = DEC-AXP

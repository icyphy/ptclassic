# Config file to build on IBM RS6000 computers running AIX 3.2 with
# IBM's xlc and xlC compilers

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
# $Id$
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

OPTIMIZER =	-O2
#WARNINGS =	-Wall -Wcast-qual
WARNINGS =
GPPFLAGS =	 -qlanglvl=compat -Daix -Daix_xlC $(MEMLOG) $(WARNINGS) $(OPTIMIZER)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =	 -Daix -Daix_xlC $(MEMLOG) $(WARNINGS) $(OPTIMIZER)

SYSLIBS= -L/usr/lpp/xlC/lib -lC -lm -lc -lbsd

#
# Variables for the linker
#
LINKER= xlC 
LINKFLAGS=-L$(LIBDIR) -bnso -bI:/lib/syscalls.exp
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

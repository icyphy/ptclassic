# Configuration makefile for Sun-4 running Cfront.
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
#		       
# Programmer:  J. T. Buck
#
# This config file suffices to compile Ptolemy under cfront version
# 2.1 on the Sun-4, and should be close to what is needed on other
# platforms.
# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------

include $(ROOT)/mk/config-default.mk

#
# Programs to use
#
RANLIB =	ranlib
# C++ compiler to use.
CPLUSPLUS = 	CC -I$(ROOT)/src/compat/cfront

# If you turn on debugging (-g) with cfront, expect ptcl and pigiRpc to be
# about 70Mb each.
GPPDEBUGFLAGS =
CDEBUGFLAGS =

# flags for C++ compilation.  -DPOSTFIX_OP= is needed for cfront 2.1; it
# is not needed for 3.0.
GPPFLAGS =	$(GPPDEBUGFLAGS) $(MEMLOG) -DPOSTFIX_OP=
CFLAGS = 	$(CDEBUGFLAGS)
DEPEND =	CC -M

#
# Variables for the linker
#

# where the Gnu library and linker is
GNULIB=
# linker to use for pigi and interpreter
LINKER =	CC
# startup module
CRT0 =
# system libraries (libraries from the environment)
SYSLIBS =	-lm

# If you don't strip symbols, then pigiRpc will be about 69Mb
# If you strip pigiRpc, then dynamic linking won't work
# The SunOS4.1.3 ld man page says:
# pigiRpc images produced with the -x ld flag will segv upon startup
# pigiRpc images produced with '-qoption ld -S' are still 69Mb

# link flags
LINKFLAGS =	-Bstatic -L$(LIBDIR)
# link flags if debugging symbols are to be left
# In cfront, this will result in a 69Mb pigiRpc
LINKFLAGS_D =	-Bstatic -L$(LIBDIR)

#
# Directories to use
#
X11_INCSPEC = -I/usr/X11/include
X11_LIBSPEC = -lX11

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR =

PURELINK =	purelink $(COLLECTOR)
PURIFY =	purelink $(COLLECTOR) purify
QUANTIFY =	purelink $(COLLECTOR) quantify

# Ptolemy will not build the s56dsp files under cfront 
#S56DIR= $(ROOT)/vendors/s56dsp
S56DIR=

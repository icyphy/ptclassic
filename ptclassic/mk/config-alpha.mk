#
# Config file to build on an Alpha AXP running DEC OSF/1, V3.2
# 
# Derived from config file for mips processor (DECstation) running Ultrix 4.x
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
# Author:  Kevin D. Kissell <kkissell@acri.fr>, Christopher Hylands
#

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Note that UC Berkeley does not formally support Ptolemy on the DEC Alpha.
# UCB does not regularly build on this platform, nor does UCB provide
# DEC Alpha Ptolemy binaries.  This is the first release of Ptolemy on the
# DEC Alpha, so there are bound to be bugs.
#
# Currently, there are problems with unaligned access that need to be cleaned
# up.  For more information, see the ultrix-osf1 FAQ, available at
# ftp://rtfm.mit.edu/pub/usenet/news.answers/dec-faq.
#

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
# Under gcc-2.7.0, you will need -fno-for-scope for GPPFLAGS
LOCALFLAGS =	-pipe -fno-for-scope
WARNINGS =	-Wall -Wcast-qual
GPPFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER) $(LOCAL_FLAGS)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER) $(LOCAL_FLAGS)

# system libraries for linking .o files from C files only
# -lots is needed to resolve _OtsDivide64Unsigned which is in the Matlab lib.
CSYSLIBS = 	-lots -lm

# system libraries (libraries from the environment)
SYSLIBS =	-lg++ $(CSYSLIBS)

#
# Variables for the linker
#
# The dynamically linked version of pigiRpc has problems that *may*
# stem from a symbol collision (clog) between libm and libg++.
#
# -depth_ring_search is necessary for incremental linking to work.
# Without -depth_ring_search, changes in a reloaded star will
# not be visible.  For more information about -depth_ring_search, see
# ld(1) and loader(5)
LINKFLAGS =	-L$(LIBDIR) -static -Xlinker -x -Xlinker -depth_ring_search
LINKFLAGS_D =	-L$(LIBDIR) -Xlinker -depth_ring_search

#
# Directories to use
#
#X11_INCSPEC =	-I$(ROOT)/src/compat
X11_LIBSPEC =	-L/usr/X11/lib -lX11

# Variable for the Ariel DSP56000 board--- only used on sun4.
S56DIR =

# Matlab architecture
MATARCH = alpha

############################################################################
# Debugging Hacks:  These really should not be necessary!

# We're not getting tk/tcl from the Ptolemy tree, it's already installed.
TK_LIBSPEC =	-L$(TCL_ROOT)/tk.$(PTARCH)/lib -ltk

# For some silly reason, the X11 library has a function linked in
# which wants to see DECnet.  Fortunately, science has a cure.
X11_LIBSPEC =	-L/usr/X11/lib -lX11 -ldnet_stub

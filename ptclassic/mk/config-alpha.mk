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

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
# include $(ROOT)/mk/config-g++.shared.mk

# Note that UC Berkeley does not formally support Ptolemy on the DEC Alpha.
# UCB does not regularly build on this platform.
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
# Under gcc-2.7.0, you will need -fno-for-scope for LOCALCCFLAGS
# -pipe might not work under DEC Alpha 'as'
LOCALCCFLAGS =	-g $(GCC_270_OPTIONS)
WARNINGS =	-Wall -Wcast-qual
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
LOCALCFLAGS =	$(LOCALCCFLAGS)
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

# system libraries for linking .o files from C files only
# -lots is needed to resolve _OtsDivide64Unsigned which is in the Matlab lib.
# To compile gcc, you may need to add -lots to CLIB in 
#  $PTOLEMY/src/gnu/src/gcc/config/alpha/x-alpha
CSYSLIBS = 	-lots -lm

# system libraries (libraries from the environment)
SYSLIBS =	-lg++ $(CSYSLIBS)

#
# Variables for the linker
#
# The dynamically linked version of pigiRpc has problems that *may*
# stem from a symbol collision (clog) between libm and libg++.
#
# If we are building static binaries, then
# -depth_ring_search is necessary for incremental linking to work.
# Without -depth_ring_search, changes in a reloaded star will
# not be visible.  For more information about -depth_ring_search, see
# ld(1) and loader(5)
# To use dlopen() style linking, we cannot use build static binaries
LINKFLAGS =	-L$(LIBDIR)
LINKFLAGS_D =	-L$(LIBDIR)

# After building the binaries, strip them or you will get a 38Mb ptcl!
# On the DEC alpha, stripping ptcl did not break incremental linking.
# The ld man page would indicate that you should be able to pass -x
# to ld, however, with -x you may see the following error:
# /usr/lib/cmplrs/cc/crt0.o: writeat_outbuf: Position 0x0 not in
#     buffer pd_outbuf
STRIP_DEBUG = /usr/ccs/bin/strip
# To turn of stripping:
#STRIP_DEBUG = true

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

# For some silly reason, the X11 library has a function linked in
# which wants to see DECnet.  Fortunately, science has a cure.
X11_LIBSPEC =	-L/usr/X11/lib -lX11 -ldnet_stub

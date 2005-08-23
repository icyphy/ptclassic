# Configuration makefile to make on an IBM RS/6000
# or PowerPC machine under AIX3.2.5 using GNU gcc and g++
#
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
# @(#)config-aix.mk	1.17 12/15/97

#	Programmer: Xavier Warzee (Thomson CSF)
#	            Alexander Kurpiers (a.kurpiers@uet.th-darmstadt.de)
#
#
# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Get the exported symbols of the Ptolemy libraries, and rules
include $(ROOT)/mk/config-aix-exp.mk

EXP_OPTS=$(subst $(LIBDIR),-Xlinker -bE:$(LIBDIR),$(LIBFILES:.a=.exp))

#
# Programs to use
#
LINKER =  g++
RANLIB =  ranlib
# Use gcc everywhere including in octtools
CC =      gcc
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC =        gcc -fwritable-strings

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

#OPTIMIZER =   -O2
OPTIMIZER =
WARNINGS =     -Wall -Wcast-qual

# If you set debugging, the expect long link times ~10 hours and large binaries
DEBUGFLAG =	#-g

CC_STATIC = #  -static
LOCALCCFLAGS = 	$(DEBUGFLAG) -DUSG -mminimal-toc $(CC_STATIC)
GPPFLAGS =     	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) $(ARCHFLAGS) \
			$(LOCALCCFLAGS) $(USERFLAGS)
LOCALCFLAGS =	-DPOSIX $(LOCALCCFLAGS)
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) $(ARCHFLAGS) \
			$(LOCALCFLAGS) $(USERFLAGS)

#
# Variables for the linker
#


# We ship statically linked binaries, but other sites might want
# to remove the -static below
LINKFLAGS =    -L$(LIBDIR) -static -liconv $(EXP_OPTS)
LINKFLAGS_D =  -L$(LIBDIR) -g -static -liconv $(EXP_OPTS)

# uncomment the following line if you have the SMT ptf installed
SMTLIB        = -Xlinker -bI:/usr/lpp/X11/bin/smt.exp

#
# Directories to use
#
X11_INCSPEC =  -I/usr/lpp/X11/include
# Some AIX X11 installations need to use the pthread library
X11_LIBSPEC =  $(SMTLIB) -L/usr/lib -lIM -L/usr/lpp/X11/lib -lX11 # -lpthread
X11EXT_LIBSPEC=-lXext

VEM_X11_LIBSPEC = \
$(SMTLIB) -L/usr/lib -lXaw -lXmu $(X11EXT_LIBSPEC) -lIM -lXt -lX11

MATLABEXT_LIB = -Xlinker \
		-bI:$(MATLABDIR)/extern/lib/$(MATARCH)/exp.$(MATARCH) \
		-L$(MATLABDIR)/extern/lib/$(MATARCH) -lmat

# system libraries (libraries from the environment)
# ordering dictates which pow function for ^ in parameter expressions is used
# and what math functions Tcl/Tk uses: math lib must come before the bsd lib
# fix sent by Alexander Kurpiers (a.kurpiers@uet.th-darmstadt.de)
SYSLIBS= -lm -lbsd

# system libraries for linking .o files from C files only
CSYSLIBS = $(SYSLIBS)

# octtools/attache uses this
TERMLIB_LIBSPEC = -lbsd

# Used to compile xv.  Use -traditional to avoid varargs problems
XV_CC =        gcc -traditional

# Matlab architecture
MATARCH = aix

# Mathematica architecture
MATHEMATICAARCH = DEC-AXP

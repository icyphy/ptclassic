# Copyright (c) 1994-1999 The Regents of the University of California.
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
# Config file to build on Linux
#
# @(#)config-linux.mk	1.46 02/22/99

# Author:  Alberto Vignani, FIAT Research Center, TORINO
# Modified by: 	Neal Becker (neal@ctd.comsat.com)
# 		Dirk Forchel (df2@inf.tu-dresden.de)
#               Wolfgang Reimer (reimer@e-technik.tu-ilmenau.de)


# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

PT_GPP_V := $(filter egcs-%,$(shell g++ --version))
ifeq ("$(PT_GPP_V)","")
    # This is GNU g++.
    include $(ROOT)/mk/config-g++.mk
else
    # This is egcs.
    include $(ROOT)/mk/config-egcs.mk
    PT_EGCS = yes
    ifeq ("$(firstword $(sort egcs-2.91.57 $(PT_GPP_V)))","egcs-2.91.57")
	# egcs 1.1 does not have optimizer bugs any longer
	USE_GPLUSPLUS =
    else
	# The versions 1.0.2 and 1.0.3a of egcs cannot compile
	# the file DEWiNeS_Tcl_Animation.cc with -O2 or higher. So we can
	# check for this error in the makefile.
	PT_EGCS_OPT_ERROR = yes
    endif
endif

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
ifndef BUILD_STATIC_BINARIES
    include $(ROOT)/mk/config-g++.shared.mk
endif

#
# Programs to use
#
RANLIB = ranlib

# Use gcc everywhere including in octtools
CC = gcc

ifeq ($(PT_EGCS),yes)
    PT_GCC_V := $(filter egcs-%,$(shell gcc --version))
    ifneq ("$(PT_GPP_V)","$(PT_GCC_V)")
	# We want to use the same compiler version (egcs) for c and c++
	# files. RH5.1 comes with egcs-1.0.2 (named g++) as c++ compiler
	# and two c compilers: the old gcc-2.7.2.3 (named gcc) end
	# egcs-1.0.2 (named egcs). 
	CC = egcs
	OCT_CC = egcs -fwritable-strings
    endif
endif

# for dynamic loading
DLLIB = -ldl

# where the Gnu library is
# GNULIB = $(PTOLEMY)/gnu/$(PTARCH)/lib
# don't set GNULIB=/usr/lib or else the wrong tcltk libraries may be used

# linker to use for pigi and interpreter.
LINKER = $(CPLUSPLUS)

ifeq ($(USE_SHARED_LIBS),yes)
    # Use Position Independent Code to build shared libraries
    C_SHAREDFLAGS = -fpic
    CC_SHAREDFLAGS = -fpic
    RANLIB = true
else
    SHARED_COMPILERDIR = $(GNULIB)
    SHARED_COMPILERDIR_FLAG = -L$(SHARED_COMPILERDIR)
    INC_LINK_FLAGS = -shared $(SHARED_COMPILERDIR_FLAG)
    SHARED_LIBRARY_PATH = $(X11_LIBDIR):$(SHARED_COMPILERDIR):$(PTOLEMY)/tcltk/tcl.$(PTARCH)/lib
    SHARED_LIBRARY_R_LIST = -Wl,-R,$(SHARED_LIBRARY_PATH)
endif

# Command to build C++ shared libraries
SHARED_LIBRARY_COMMAND = $(CC) -shared $(SHARED_COMPILERDIR_FLAG) -o
 
# Command to build C shared libraries
CSHARED_LIBRARY_COMMAND = $(CC) -shared $(SHARED_COMPILERDIR_FLAG) -o

# linker for C utilities.  If we are using shared libraries, then
# we want to avoid involving libg++.so, so we use gcc to link.
CLINKER = $(CC)

# domains/ipus/islang uses BISONFLEXLIBS
BISONFLEXLIB =	-fl

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

#	Don't use -pipe, it makes life worse on small-memory systems.
#	Don't use -m486, it's the default, except for those with the
#	Pentium optimized compiler; for them -m486 makes things worse.
#OPTIMIZER =	-m486 -pipe
ifeq ($(PT_EGCS),yes)
    OPTIMIZER =	-O3 -march=i486 -mcpu=pentiumpro -fomit-frame-pointer# -pipe
else
    OPTIMIZER =	-O3 -fomit-frame-pointer# -pipe
endif

# -Wsynth is a g++ flag first introduced in g++-2.6.x., however 2.5.x does not support it
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wcast-align -Wsynth# -Wcast-qual 

# need _REENTRANT to get thread aware code
ARCHFLAGS =	-Dlinux -D_REENTRANT

# need _PTHREAD_1003_1c for PosixThread.cc to conform with POSIX 1003.1c
# need EXTRAOPTS for pigiLoader.cc to compile user stars with same optimization
LOCALCCFLAGS =  -D_PTHREAD_1003_1c -DEXTRAOPTS="\"$(OPTIMIZER) -fpic\""
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)

# need -DI_UNISTD for rman to find the declaration of getopt()
LOCALCFLAGS =   -DI_UNISTD
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)


#
# Variables for the linker
#
# system libraries for linking .o files from C files only
CSYSLIBS=$(SHARED_COMPILERDIR_FLAG) -lm $(DLLIB) #-lieee

# system libraries (libraries from the environment)
# we don't need -lg++: old GNU g++ links with -lg++ automatically and
# egcs does not need libg++ any longer.
SYSLIBS=$(CSYSLIBS) #-lg++


# Ask ld to strip symbolic information, otherwise, expect a 32Mb pigiRpc
LINKSTRIPFLAGS=-Wl,-s

#LINKFLAGS=-L$(LIBDIR) $(LINKSTRIPFLAGS)# -static
#LINKFLAGS_D=-L$(LIBDIR) -g -static
LINKFLAGS=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST) $(LINKSTRIPFLAGS) -rdynamic # -static
LINKFLAGS_D=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST) -g -rdynamic #-static

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap
CURSES_LIBSPEC = -lncurses
# You may need to do something like:
#CURSES_LIBSPEC = -lcurses

#
# Variables for miscellaneous programs
#
# Used by xv
# -DXLIB_ILLEGAL_ACCESS is need for X11R6 to compile xv.c:rd_str_cl()
XV_CC =		$(CC) -DXLIB_ILLEGAL_ACCESS $(X11_INCSPEC) $(X11_LIBSPEC)
# You may need to do something like:
# XV_CC =	$(CC) -DXLIB_ILLEGAL_ACCESS $(X11_INCSPEC) $(X11_LIBSPEC) \
#               -traditional -DDIRENT
#
# Directories to use
#
# 'standard' (SLS-Debian) locations
#
#X11_INCSPEC = -I/usr/X386/include/X11
#X11INCL     = -I/usr/X386/include/X11
#X11_LIBSPEC = -L/usr/X386/lib/X11 -lX11
#
# X11R6
#
#X11_INCSPEC =	-I$(ROOT)/src/compat -I/usr/sww/X11R6/include
#X11_LIBSPEC =	-L/usr/sww/X11R6/lib -lX11
# Use -lSM -lICE for X11R6, don't use then for X11R5
X11EXT_LIBSPEC=-lXext -lSM -lICE
#
# Slackware >=1.2 locations
#
#X11INCL     = -I/usr/X11/include
X11_INCSPEC = -I/usr/X11R6/include
X11_LIBDIR  = /usr/X11R6/lib
X11_LIBSPEC = -L$(X11_LIBDIR) -lX11

# Flag that cc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = #-static

# Matlab architecture
MATARCH = lnx86

# If you have MATLAB then you can set the environment variable MATLABDIR
# to the MATLAB root directory.
ifdef MATLABDIR
    # Check if MATLABDIR points to a MATLAB directory
    # MATLAB 4 has a static libmat.a whereas MATLAB 5 has a DLL libmat.so 
    INCLUDE_MATLAB := $(shell \
	if [ -r "$(MATLABDIR)/extern/lib/$(MATARCH)/libmat.a" -o -r "$(MATLABDIR)/extern/lib/$(MATARCH)/libmat.so" ]; \
	then echo yes; else echo no; echo \
	'Warning: $$MATLABDIR is not a MATLAB root dir! Ignoring setting.' \
	>&2; fi)
else
    INCLUDE_MATLAB = no
endif
# Shared library option to search MATLAB's Linux DLLs
MATLABEXTRAOPTS = -Wl,-R,$(MATLABDIR)/sys/$(MATARCH)

# If you have Mathematica then you can set the environment variable
# MATHEMATICADIR to the Mathematica root directory.
ifdef MATHEMATICADIR
    # FIXME: One should check if MATHEMATICADIR points to the right location
    INCLUDE_MATHEMATICA = yes
else
    INCLUDE_MATHEMATICA = no
endif
INCLUDE_PN_DOMAIN = no


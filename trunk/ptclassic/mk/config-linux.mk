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
# Config file to build on Linux
#
# $Id$

# Author:  Alberto Vignani, FIAT Research Center, TORINO
# Modified by: 	Neal Becker (neal@ctd.comsat.com)
# 		Dirk Forchel (df2@inf.tu-dresden.de)
#               Wolfgang Reimer (reimer@e-technik.tu-ilmenau.de)

#
# --------------------------------------------------------------------
# |  Please note that Linux is moving to the ELF object file format, |
# |  but only some new distributions support it as of May 8,1995     |
# |  e.g.the Slackware distribution up to 2.2 does NOT support ELF   |
# --------------------------------------------------------------------
# Answer 'yes' if you have installed ELF tools (libc 5.0.x, binutils
#	2.5.2l.xx, libg++-2.6.2.4, ELF gcc 2.6.3 or later), even if
#	you compile in a.out format !
#
HAVE_ELF=yes
#
# We use ELF.
#
USE_ELF=yes
#
# We use a.out.
#USE_ELF=no

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

include $(ROOT)/mk/config-g++.mk

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
ifndef BUILD_STATIC_BINARIES
include $(ROOT)/mk/config-g++.shared.mk
endif

#
# Programs to use
#
RANLIB = 	ranlib
# Use gcc everywhere including in octtools
CC =		gcc

#
# Programs to use - override previous definitions
#
ifeq ($(HAVE_ELF),yes)
ifeq ($(USE_ELF),yes)
DLLIB	=	-ldl
else
AS	=	/usr/i486-linuxaout/bin/as
LD	=	/usr/i486-linuxaout/bin/ld -m i386linux
CC	=	gcc -b i486-linuxaout
OCT_CC	=	gcc -b i486-linuxaout -fwritable-strings
CPP	=	gcc -E
CPLUSPLUS = 	g++ -b i486-linuxaout
endif
endif

DEPEND= $(CPLUSPLUS) -MM

# where the Gnu library is
GNULIB= /usr/lib #$(PTOLEMY)/gnu/$(PTARCH)/lib

# linker to use for pigi and interpreter.
LINKER	= $(CPLUSPLUS)

# Location of GNU libg++ shared libraries
SHARED_COMPILERDIR =	/usr/lib#$(PTOLEMY)/gnu/$(PTARCH)/lib
SHARED_COMPILERDIR_FLAG = -L$(SHARED_COMPILERDIR)

# Command to build C++ shared libraries
SHARED_LIBRARY_COMMAND = $(CPLUSPLUS) -shared $(SHARED_COMPILERDIR_FLAG) -o
 
# Command to build C shared libraries
CSHARED_LIBRARY_COMMAND = $(CC) -shared $(SHARED_COMPILERDIR_FLAG) -o

# linker for C utilities.  If we are using shared libraries, then
# we want to avoid involving libg++.so, so we use gcc to link.
CLINKER = $(CC)

# These are the additional flags that we need when we are compiling code
# which is to be dynamically linked into Ptolemy.  -shared is necessary
# with gcc-2.7.0
INC_LINK_FLAGS = -shared $(SHARED_COMPILERDIR_FLAG)
 
# List of libraries to search, obviating the need to set LD_LIBRARY_PATH
# See the ld man page for more information.  These path names must
# be absolute pathnames, not relative pathnames.
SHARED_LIBRARY_PATH = $(PTOLEMY)/lib.$(PTARCH):$(PTOLEMY)/octtools/lib.$(PTARCH):$(SHARED_COMPILERDIR):$(PTOLEMY)/tcltk/tcl.$(PTARCH)/lib/shared:$(PTOLEMY)/tcltk/tk.$(PTARCH)/lib/shared/:$(PTOLEMY)/tcltk/itcl.$(PTARCH)/lib/shared
SHARED_LIBRARY_R_LIST = -Wl,-R,$(SHARED_LIBRARY_PATH)

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
#OPTIMIZER =	-g #-m486 -pipe
OPTIMIZER =	-O2 #-fomit-frame-pointer #-m486 -pipe
# -Wsynth is new in g++-2.6.x, however 2.5.x does not support it
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
WARNINGS =	-Wall -Wcast-align -Wsynth # -Wcast-qual 
# you will need -DI_UNISTD for rman to find the declaration of getopt()
ARCHFLAGS =	-Dlinux -DI_UNISTD#-D_GNU_SOURCE -D_BSD_SOURCE -DNO_RAND_OPTIMIZE
# Under gcc-2.7.0, you will need -fno-for-scope for LOCALCCFLAGS
LOCALCCFLAGS =	-fno-for-scope
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
LOCALCFLAGS =
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)


#
# Variables for the linker
#
# system libraries for linking .o files from C files only
CSYSLIBS =	-lieee -lm $(DLLIB)

# system libraries (libraries from the environment)
SYSLIBS =	$(CSYSLIBS) $(SHARED_COMPILERDIR_FLAG) -lg++


# Ask ld to strip symbolic information, otherwise, expect a 32Mb pigiRpc
LINKSTRIPFLAGS=-Wl,-S,-x

#LINKFLAGS=-L$(LIBDIR) $(LINKSTRIPFLAGS)# -static
#LINKFLAGS_D=-L$(LIBDIR) -g -static
LINKFLAGS=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST) $(LINKSTRIPFLAGS) -rdynamic # -static
LINKFLAGS_D=-L$(LIBDIR) $(SHARED_LIBRARY_R_LIST) -g -rdynamic #-static

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap

#
# Variables for miscellaneous programs
#
# Used by xv
# -DXLIB_ILLEGAL_ACCESS is need for X11R6 to compile xv.c:rd_str_cl()
XV_CC =		$(CC) -DXLIB_ILLEGAL_ACCESS $(X11_INCSPEC) $(X11_LIBSPEC)

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
X11_INCSPEC = -I/usr/X11/include
X11INCL     = -I/usr/X11/include
X11_LIBSPEC = -L/usr/X11/lib -lX11

# Flag that cc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = #-static

# Matlab architecture
MATARCH = lnx86 #i486

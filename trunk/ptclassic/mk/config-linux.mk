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

# Author: Alberto Vignani, FIAT Research Center, TORINO

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
#USE_ELF=yes
#
# We use a.out.
USE_ELF=no

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

include $(ROOT)/mk/config-g++.mk

#
# Programs to use
#
RANLIB = 	ranlib
CC =		gcc
# If we have g++, then compile Octtools with gcc.  ARCHs that are cfront
# based probably don't have gcc.
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC =	gcc

#
# Programs to use - override previous definitions
#
ifeq ($(HAVE_ELF),yes)
ifeq ($(USE_ELF),yes)
CC	=	gcc
# If we have g++, then compile Octtools with gcc.  ARCHs that are cfront
# based probably don't have gcc.
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
# define in this way to avoid recursion
OCT_CC	=	gcc
CPLUSPLUS =	g++
LD	=	ld -m elf_i386
CPP	=	gcc -E -D__ELF__
OBJDUMP	=	objdump
OBJDUMP_FLAGS =	-k -q 
LDFLAGS	=	-e startup_32 
else
AS	=	/usr/i486-linuxaout/bin/as
LD	=	/usr/i486-linuxaout/bin/ld -m i386linux
CC	=	gcc -b i486-linuxaout
OCT_CC	=	gcc -b i486-linuxaout
CPP	=	gcc -E
CPLUSPLUS = 	g++ -b i486-linuxaout
endif
else
CC	=	gcc
OCT_CC	=	gcc
CPP	=	gcc -E
AS	=	as
LD	=	ld
CPLUSPLUS = 	g++
endif

LINKER	=	$(CPLUSPLUS)
RANLIB	= 	ranlib

# debug version
LINUXDEF =	-Dlinux #-D_GNU_SOURCE -D_BSD_SOURCE
#	Don't use -pipe, it makes life worse on small-memory systems.
#	Don't use -m486, it's the default, except for those with the
#	Pentium optimized compiler; for them -m486 makes things worse.
#OPTIMIZER =	-g #-m486 -pipe

#
# 'production' version with optimization
OPTIMIZER =	-O2 #-fomit-frame-pointer #-m486 -pipe
#LINUXDEF =	-Dlinux -DNO_RAND_OPTIMIZE #-D_GNU_SOURCE -D_BSD_SOURCE

# -Wsynth is new in g++-2.6.x, however 2.5.x does not support it
# Slackware is using 2.5.x, so we leave -Wsynth out for the time being.
WARNINGS =	-Wall -Wcast-qual -Wcast-align # -Wsynth
GPPFLAGS =	$(LINUXDEF) $(WARNINGS) $(OPTIMIZER) $(MEMLOG)
CFLAGS =	$(LINUXDEF) $(OPTIMIZER) -fwritable-strings

#
# Variables for the linker
#
# system libraries (libraries from the environment)
SYSLIBS=-lg++ -lieee -lm

LINKFLAGS=-L$(LIBDIR) -Xlinker -S -Xlinker -x # -static
LINKFLAGS_D=-L$(LIBDIR) -g -static

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap

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


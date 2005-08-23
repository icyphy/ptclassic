#
# Default configuration for Ptolemy builds.  Every Ptolemy makefile
# includes one of the config-xxx.mk files in this directory (where
# "xxx" might be "sun4" or "mips", for example).  Every such config file
# include this default config, and then selectively over-rides options
# defined here.  This is the master "definition" of all the makefile
# config variables, and the master documentation for what the variables
# mean.  The comments included here should *not* be duplicated into
# the other config files because then they become inconsistent and
# out of date.
#
# There are three groups of config variables: paths, options and programs

# @(#)config-default.mk	1.67a 09/20/99
# Copyright (c) 1990-1999 The Regents of the University of California.
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
# Authors:  J. T. Buck, Kennard White

# NOTE: you can't use GNU make extensions in this file, or bootstrapping
# with non-GNU makes will fail.

# Each makefile must define ROOT, the relative position of the root
# of the directory tree, and include one of the makefiles file.
# PTARCH is assumed set to, say, "sun4", "sun3", or "vax".

#-----------------------------------------------------------------------
# 		Directories to use
#-----------------------------------------------------------------------

# where libraries are (and where to install them)
LIBDIR=$(ROOT)/lib.$(PTARCH)
# where to install binaries
BINDIR=$(ROOT)/bin.$(PTARCH)
# Ptolemy kernel directory
KERNDIR=$(ROOT)/src/kernel
# where help files go
HELPDIR=$(ROOT)/lib

PTCL_OBJS = $(OBJDIR)/ptcl/PTcl.o
PTCL_INCDIR = $(ROOT)/src/ptcl

# root of Octtools
OCTTOOLS = $(ROOT)/octtools
# Oct libraries
OCTLIBDIR = $(OCTTOOLS)/lib.$(PTARCH)
# cc to compile octtools with.
OCT_CC =	cc
# octtools/attache uses these two.
TERMLIB_LIBSPEC = -ltermlib
CURSES_LIBSPEC = -lcurses
# hppa.cfront has problems linking vem with -g, so we override this
OCT_DEBUG_FLAGS = -g

# OCTTOOLS_ATTACHE_DIR is usually set to attache in
# $PTOLEMY/mk/config-default.mk
# but config-nt4.mk overrides it and sets it to nothing because Cygwin
# cannot compile attache because of curses issues
OCTTOOLS_ATTACHE_DIR = attache

# OCTTOOLS_IV_DIR is usually set to iv in $PTOLEMY/mk/config-default.mk
# but config-nt4.mk overrides it and sets it to nothing because Cygwin
# cannot compile iv because of signal issues.
OCTTOOLS_IV_DIR = iv

# If the X11 include directories are in /usr/include/X11, leave
# the following symbol blank.  Otherwise define it as "-Idir" where
# dir is the PARENT of the include directory, which must end in X11.
X11_INCSPEC=

# Architectures that build with shared libraries use this to set the 
# library search path
X11_LIBDIR=/usr/X11/lib

# The following has two parts.  The first part is optional, and
# is only required if the X libraries are not on the default search path.
# If this is the case, add something like "-Ldir" where dir is the directory
# containing them.  The second part is normally "-lX11", but might be
# different if your vendor has put it elsewhere (e.g., "-lX11-mit").
X11_LIBSPEC= -lX11
#X11_LIBSPEC= -L/usr/X11/lib -lX11

# This is just like X11_LIBSPEC, but refers to the X11 extension library.
# Normally this is "-lXext", but might be different if your vendor has
# put it elsewhere (e.g., "-lXext-mit").
X11EXT_LIBSPEC = -lXext
#X11EXT_LIBSPEC = -L/usr/X11/lib -lXext

TCL_ROOT=$(ROOT)/tcltk

TCL_VERSION_NUM=8.0
TK_VERSION_NUM=8.0
ITCL_VERSION_NUM=3.0
ITCL_MINOR_VERSION_NUM=a1
#ITCL_SRC_INCDIR =-I$(ROOT)/src/tcltk/tcl$(ITCL_VERSION_NUM)$(ITCL_MINOR_VERSION_NUM)/tcl8.0/generic

# Combined -L and -l options to link with tcl library.
TCL_LIBSPEC=-L$(TCL_ROOT)/tcl.$(PTARCH)/lib -ltcl$(TCL_VERSION_NUM)

# Combined -L and -l options to link with tk library.  Can add
# addtional -L and/or -l options to support tk extensions.
TK_LIBSPEC=-L$(TCL_ROOT)/tcl.$(PTARCH)/lib/ -ltk$(TK_VERSION_NUM) #-lXpm

# Directory containing Tcl include files
TCL_INCDIR=$(TCL_ROOT)/tcl/include

# Directory containing Tk include files
TK_INCDIR=$(TCL_ROOT)/tcl/include


# Directory containing itcl include files
ITCL_INCDIR=$(TCL_ROOT)/tcl/include
ITCL_LIBSPEC=-L$(TCL_ROOT)/tcl.$(PTARCH)/lib \
	-litcl$(ITCL_VERSION_NUM)

ITK_INCDIR=$(TCL_ROOT)/tcl/include
ITK_LIBSPEC=-L$(TCL_ROOT)/tcl.$(PTARCH)/lib \
	-litk$(ITCL_VERSION_NUM)

# Location of the itcl library, needed for itcl_mkindex
ITCL_LIBDIR=$(ROOT)/tcltk/tcl/library

#-----------------------------------------------------------------------
# 		Global options to use
#-----------------------------------------------------------------------
# To enable memory logging, define the following symbol as -DMEMORYLOG.
# By default it is disabled.  To enable, uncomment the following line
# (or define it in the arch-config mk file).
#MEMLOG = -DMEMORYLOG

#-----------------------------------------------------------------------
# 		Programs and flags to use
#-----------------------------------------------------------------------
# C++ compiler to use
CPLUSPLUS = g++

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

# C++ compiler flags.  Defined by the arch-config mk.
GPPFLAGS = $(MEMLOG)
# C compiler flags.  Defined by the arch-config mk.
CFLAGS =

# Archive tool to use
AR = ar
# Command to use when getting files from SCCS (Gnu make)
GET=safe-get

# yacc is used by ptlang and islang
YACC = yacc

# lex is used by islang on the hp
LEX = flex

# If you are running bison, you may safely remove -ly from BISONFLEXLIBS
# domains/ipus/islang uses BISONFLEXLIBS
# If you are running flex, you may want to try:
# BISONFLEXLIBS = 	-fl
BISONFLEXLIBS = 	-ly -ll 

# command to generate dependencies (cfront users can try CC -M)
DEPEND=g++ -MM

# linker to use for pigi and interpreter.
LINKER=g++
# linker for C utilities.  If we are using shared libraries, then
# we want to avoid involving libg++.so, so we use gcc to link.
CLINKER=$(LINKER)
# system libraries (libraries from the environment)
SYSLIBS=-lm
# system libraries for linking .o files from C files only
CSYSLIBS=-lm

# linker options.  Defined by the arch-config mk.
LINKFLAGS=-L$(LIBDIR)
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR)

# On certain archs, we can run strip and end up with a smaller binary
# Note that doing a full strip on a binary will disable incremental linking
# Some archs support 'strip -x' which removed only the local symbols
STRIP_DEBUG= true

# Default is to build non-shared (or static) libraries
C_SHAREDFLAGS =
CC_SHAREDFLAGS =

# mk/userstars.mk uses these vars
USER_C_SHAREDFLAGS =	$(C_SHAREDFLAGS)
USER_CC_SHAREDFLAGS =	$(CC_SHAREDFLAGS)

# Suffix for object files
OBJSUFFIX =	o

# Libraries are of the form libfoo.$(LIBSUFFIX) 
LIBSUFFIX = 	a
LIBNONSHAREDSUFFIX = a

# Flag for cc to statically link binaries (hppa and sun4 currently use this)
CC_STATIC=

# Used to build xv.  xv is distributed in the other.src tar file
XV_RAND=
XV_INSTALL=install

# Directory for general compatiblity include files.  Mainly function
# declarations to quiet gcc -Wall down
PTCOMPAT_INCSPEC =	-I$(ROOT)/src/compat/ptolemy


# Tcl file that makes generic tycho indices
TYCHOMAKEINDEX =	$(ROOT)/tycho/lib/idx/tychoMakeIndex.tcl
# Tcl file that makes star indices
STARINDEX =		$(ROOT)/tycho/typt/lib/starindex.tcl


# ptbin.mk uses this to decide whether to include the Ipus stars
INCLUDE_IPUS_DOMAIN = no

# ptbin.mk uses this to decide whether to include the PN stars
INCLUDE_PN_DOMAIN = no

# src/thread/makefile uses this to decide whether to build gthreads
# gthreads should be built only on sol2, sol2.cfront and sun4.
INCLUDE_GTHREADS = no

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

# $Id$
# Copyright (c) 1991 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Authors:  J. T. Buck, Kennard White

# Each makefile must define ROOT, the relative position of the root
# of the directory tree, and include one of the makefiles file.
# ARCH is assumed set to, say, "sun4", "sun3", or "vax".

#-----------------------------------------------------------------------
# 		Directories to use
#-----------------------------------------------------------------------

# Full path to the ~ptolemy directory (you may need to change this
# for your installation)
PTOLEMY_FULL_PATH = /usr/users/ptolemy

# where libraries are (and where to install them)
LIBDIR=$(ROOT)/lib.$(ARCH)
# where to install binaries
BINDIR=$(ROOT)/bin.$(ARCH)
# Ptolemy kernel directory
KERNDIR=$(ROOT)/src/kernel
# where help files go
# default place to move documentation generated by ptlang.
STARDOCDIR = $(VPATH)/../doc/stars
HELPDIR=~ptolemy/lib

PTCL_OBJS = $(OBJDIR)/ptcl/PTcl.o
PTCL_INCDIR = $(ROOT)/src/ptcl

# root of Octtools
OCTTOOLS = $(ROOT)/octtools
# Oct libraries
OCTLIBDIR = $(OCTTOOLS)/lib.$(ARCH)
# cc to compile octtools with.
OCT_CC =	cc

# If the X11 include directories are in /usr/include/X11, leave
# the following symbol blank.  Otherwise define it as "-Idir" where
# dir is the PARENT of the include directory, which must end in X11.
X11_INCSPEC=

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

# Directory containing Tcl include files
TCL_INCDIR=$(TCL_ROOT)/tcl/include

# Combined -L and -l options to link with tcl library.
TCL_LIBSPEC=-L$(TCL_ROOT)/tcl.$(ARCH)/lib -ltcl

# Directory containing Tk include files
TK_INCDIR=$(TCL_ROOT)/tk/include

# Combined -L and -l options to link with tk library.  Can add
# addtional -L and/or -l options to support tk extensions.
TK_LIBSPEC=-L$(TCL_ROOT)/tk.$(ARCH)/lib -ltk -lXpm

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
# C++ compiler flags.  Defined by the arch-config mk.
GPPFLAGS = $(MEMLOG)
# C compiler flags.  Defined by the arch-config mk.
CFLAGS =

# command to use when getting files from SCCS (Gnu make)
GET=safe-get

# command to generate dependencies (cfront users can try CC -M)
DEPEND=g++ -MM

# linker to use for pigi and interpreter.
LINKER=g++
# system libraries (libraries from the environment)
SYSLIBS=-lg++ -lm
# linker options.  Defined by the arch-config mk.
LINKFLAGS=-L$(LIBDIR)
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR)

# Default is to build non-shared (or static) libraries
C_SHAREDFLAGS =
CC_SHAREDFLAGS =

# Libraries are of the form libfoo.$(LIBSUFFIX) 
LIBSUFFIX = 	a

# Flag for cc to statically link binaries (hppa and sun4 currently use this)
CC_STATIC=

# Used to build xv.  xv is distributed in the other.src tar file
XV_RAND=
XV_INSTALL=install

# Used for Matlab's external interface; by default, Matlab is not installed
# -- If Matlab is installed, then MATLABDIR points to where MATLAB is installed 
#    and MATLABLIBDIR points to the directory containing the Matlab libraries
# -- If Matlab is not installed, then MATLABDIR equals $ROOT/src/compat/matlab
#    and MATLABLIBIDR is undefined
MATLABDIR= $(ROOT)/src/compat/matlab

# Directory for general compatiblity include files.  Mainly function
# declarations to quiet gcc -Wall down
PTCOMPAT_INCSPEC =	-I$(ROOT)/src/compat/ptolemy

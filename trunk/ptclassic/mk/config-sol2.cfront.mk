# Configuration makefile for Sun running Solaris2.3 and Sun CC3.0, CC4.0
# (Note that CC4.0 is not a cfront style compiler, it is native.)
#
# $Id$
# Copyright (c) 1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmers:  Richard Tobias, Christopher Hylands
#
# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------

include $(ROOT)/mk/config-default.mk

#
# Programs to use
#
RANLIB =	true
# C++ compiler to use.
CPLUSPLUS = 	CC -I$(ROOT)/src/compat/cfront

# common.mk looks at this variable to decide how to build shared libraries
USE_SHARED_LIBS = yes
#USE_SHARED_LIBS = no

# Using GNU make conditionals causes havoc while bootstrapping gcc,
# so we don't use them here, however, this is what the code would look like

#ifeq ($(USE_SHARED_LIBS),yes) 
# Use Position Independent Code to build shared libraries
# Octtools/Xpackages/rpc requires PIC instead of pic
C_SHAREDFLAGS =		-K PIC
# libcgstars.a requires PIC instead of pic
CC_SHAREDFLAGS =	-PIC
LIBSUFFIX =		so
SHARED_LIBRARY_COMMAND =	CC -G -o
#endif

# If you turn on debugging (-g) with cfront, ptcl and pigiRpc could be
# about 70Mb each.  Also, with -g you will need at least 250Mb for the .o
# files, even before linking
#
# If you don't have the -g flag turned on then Sun CC 3.0.1 fails to compile 
# kernel/PortHole.cc with messages about: 
#	operands have incompatible types: op ":" 
GPPDEBUGFLAGS = -g
CDEBUGFLAGS =	-g

# flags for C++ compilation.  -DPOSTFIX_OP= is needed for cfront 2.1; it
# is not needed for 3.0.
OPTIMIZER =
# Define PTSOL2_4 if you are on Solaris2_4
LOCALFLAGS =	#-DPTSOL2_4
GPPFLAGS =	-DSYSV -DSOL2 $(OPTIMIZER) $(GPPDEBUGFLAGS) $(MEMLOG) -DPOSTFIX_OP= $(LOCACLFLAGS)
CFLAGS = 	-DSYSV -DSOL2 $(OPTIMIZER) $(CDEBUGFLAGS) $(LOCALFLAGS)
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
SYSLIBS =	-lsocket -lnsl -ldl -lm

# If you don't strip symbols, then pigiRpc will be about 69Mb
# If you strip pigiRpc, then dynamic linking won't work
# The SunOS4.1.3 ld man page says:
# pigiRpc images produced with the -x ld flag will segv upon startup
# pigiRpc images produced with '-qoption ld -S' are still 69Mb

# Can't use -Bstatic here, or we won't be able to find -ldl, and
# dynamic linking will not work.
LINKFLAGS=-L$(LIBDIR) -R $(PTOLEMY)/lib.$(ARCH):$(PTOLEMY)/octtools/lib.$(ARCH):$(X11_LIBDIR)
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) -R $(PTOLEMY)/lib.$(ARCH):$(PTOLEMY)/octtools/lib.$(ARCH):$(X11_LIBDIR)	

#
# Directories to use
#
X11_INCSPEC =	-I/usr/openwin/include
X11_LIBDIR =	/usr/openwin/lib
X11_LIBSPEC =	-L$(X11_LIBDIR)  -lX11
X11EXT_LIBSPEC = -L$(X11_LIBDIR) -lXext

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR =

PURELINK =
PURIFY =	$(COLLECTOR) purify
QUANTIFY =

# Ptolemy will not build the s56dsp files under cfront 
#S56DIR= $(ROOT)/vendors/s56dsp
S56DIR=

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE -DSYSV $(X11_INCSPEC)

# Defines to build xv
XMKMF =		/usr/openwin/bin/xmkmf
# -Xs is needed for the varargs code in xv/tiff
# -w turns of warnings.  xv/bitmaps.h causes _lots_ of warnings
# -R$(X11LIB_DIR) is need so we can find the X libs at runtime,
#   otherwise, we will need to set LD_LIBRARY_PATH to include $(X11_LIBDIR) 
XV_CC =		cc -Xs -w $(X11_INCSPEC) \
		-DSVR4 -DSYSV -DDIRENT -DATT -DNO_BCOPY \
		$(X11_LIBSPEC) -R$(X11_LIBDIR)
XV_RAND = 	-DNO_RANDOM

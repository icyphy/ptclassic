#
# Config file to build on sun4 processor (SparcStation) running Solaris2.3
# with gcc and g++

# @(#)config-sol2.mk	1.10 2/2/95

# Copyright (c) 1994 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

#
# Programs to use
#
RANLIB = true
# Use gcc everywhere including in octtools
CC =		gcc

# Using GNU make conditionals causes havoc while bootstrapping gcc,
# so we don't use them here, however, this is what the code would look like

# common.mk looks at this variable to decide how to build shared libraries
#USE_SHARED_LIBS = no
#
#ifeq ($(USE_SHARED_LIBS),yes) 
# Use Position Independent Code to build shared libraries
# Not yet supported on Solaris2 with g++
#C_SHAREDFLAGS =	-fPIC
#CC_SHAREDFLAGS =	-fPIC
# mk/userstars.mk uses these vars
#USER_C_SHAREDFLAGS =	$(C_SHAREDFLAGS)
#USER_CC_SHAREDFLAGS =	$(CC_SHAREDFLAGS)
#LIBSUFFIX =		so
#endif

OPTIMIZER =	
#-Wsynth is new in g++-2.6.x
WARNINGS =	-Wall -Wcast-qual -Wsynth
MULTITHREAD =	-D_REENTRANT
GPPFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER) $(MULTITHREAD)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER) $(MULTITHREAD)

#
# Variables for the linker
#
# system libraries (libraries from the environment)
SYSLIBS=-lsocket -lnsl -ldl -lg++ -lm

# Ask ld to strip symbolic information, otherwise, expect a 32Mb pigiRpc
LINKSTRIPFLAGS=-Wl,-s

# Can't use -static here, or we won't be able to find -ldl, and
# dynamic linking will not work.
LINKFLAGS=-L$(LIBDIR) -Wl,-R,$(PTOLEMY)/lib.$(ARCH):$(PTOLEMY)/octtools/lib.$(ARCH):$(X11_LIBDIR) $(LINKSTRIPFLAGS)
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) -Wl,-R,$(PTOLEMY)/lib.$(ARCH):$(PTOLEMY)/octtools/lib.$(ARCH):$(X11_LIBDIR)

# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that cc uses -Bstatic
CC_STATIC =

#
# Directories to use
#
X11_INCSPEC =	-I/usr/openwin/include
X11_LIBDIR =	/usr/openwin/lib
X11_LIBSPEC =	-L$(X11_LIBDIR)  -lX11

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR = 	

PURELINK =	
PURIFY = 	purify -cache-dir=/var/pure/cache
QUANTIFY = 	quantify	

# Variable for the Ariel DSP56000 board
S56DIR =

# Variables for local Matlab installation
# -- If Matlab is installed, then MATLABDIR points to where MATLAB is installed
#    and MATLABLIBDIR points to the directory containing the Matlab libraries
# -- If Matlab is not installed, then MATLABDIR equals $ROOT/src/compat/matlab
#    and MATLABLIBIDR is undefined
#MATLABDIR =	/usr/sww/matlab
#MATLABLIBDIR =	-L$(MATLABDIR)/extern/lib/$(ARCH)
MATLABDIR =	$(ROOT)/src/compat/matlab
MATLABLIBDIR =

#
# Variables for miscellaneous programs
#
# Used by xv
#XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =

# -DATT is needed so we don't try and include sys/dir.h
# -R$(X11LIB_DIR) is need so we can find the X libs at runtime,
#	otherwise, we will need to set LD_LIBRARY_PATH
XV_CC =		gcc -traditional $(X11_INCSPEC) \
		-DSVR4 -DSYSV -DDIRENT -DATT -DNO_BCOPY \
		$(X11_LIBSPEC) -R$(X11_LIBDIR)

XV_RAND = 	-DNO_RANDOM

# Under sol2, xmkmf is not compatible with gcc, so we don't use it
XMKMF =		rm -f Makefile; cp Makefile.std Makefile

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE $(X11_INCSPEC)

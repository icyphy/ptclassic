# Makefile to build shared library octtools libraries for SGI Irix5
#
# $Id$
# Copyright (c) 1995 The Regents of the University of California.
#                       All Rights Reserved.
#	Programmer: Christopher Hylands

# If we are building on the irix5, build octtools libraries as shared
# libraries.  If octtools is built as static libraries, then the GOT will
# overflow when linking a full blown pigiRpc.  See the Irix dso man page for
# more information. 

# common.mk looks at this variable to decide how to build shared libraries
USE_SHARED_LIBS = yes
#USE_SHARED_LIBS = no

# Using GNU make conditionals causes havoc while bootstrapping gcc,
# so we don't use them here, however, this is what the code would look like

#ifeq ($(USE_SHARED_LIBS),yes) 
# Use Position Independent Code to build shared libraries
# Octtools/Xpackages/rpc requires PIC instead of pic
C_SHAREDFLAGS =		-fPIC
# libcgstars.a requires PIC instead of pic
CC_SHAREDFLAGS =	-fPIC
LIBSUFFIX =		so
SHARED_LIBRARY_COMMAND =	ld -shared -exports -all -o
#endif


# Fiendishly evil hack to get change the LIB from libfoo.a to libfoo.so
TMPLIB := 		$(LIB)
LIB = 			$(basename $(TMPLIB)).$(LIBSUFFIX)


# Makefile to build shared library octtools libraries for SGI Irix5
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


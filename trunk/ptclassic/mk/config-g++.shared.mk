# Config to use for g++ with shared libraries.
# This is not a complete config; it only overrides those options
# specific to using g++.  See also config-g++.mk
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
#		       
# Programmers:  Christopher Hylands, Jose Pino

# Using GNU make conditionals causes havoc while bootstrapping gcc,
# so we don't use them here, however, this is what the code would look like

# common.mk looks at this variable to decide how to build shared libraries
USE_SHARED_LIBS = yes
#
#ifeq ($(USE_SHARED_LIBS),yes) 
# Use Position Independent Code to build shared libraries
C_SHAREDFLAGS =		-fPIC
CC_SHAREDFLAGS =	-fPIC
# mk/userstars.mk uses these vars
USER_C_SHAREDFLAGS =	$(C_SHAREDFLAGS)
USER_CC_SHAREDFLAGS =	$(CC_SHAREDFLAGS)
LIBSUFFIX =		so
#endif

# Location of GNU libg++ shared libraries
SHARED_COMPILERDIR =	$(PTOLEMY)/gnu/$(PTARCH)/lib/shared
SHARED_COMPILERDIR_FLAG = -L$(SHARED_COMPILERDIR)

# Command to build shared libraries (Not really supported yet)
SHARED_LIBRARY_COMMAND = g++ -shared $(SHARED_COMPILERDIR_FLAG) -o

# Used by cgwork.mk
INC_LINK_FLAGS =	-shared $(SHARED_COMPILERDIR_FLAG)

TCL_LIBSPEC =	$(TCL_SHARED_LIBSPEC)
TK_LIBSPEC =	$(TK_SHARED_LIBSPEC)
#ITCL_LIBSPEC =	$(ITCL_SHARED_LIBSPEC)
# List of libraries to search, obviating the need to set LD_LIBRARY_PATH
# See the ld man page for more information.  These path names must
# be absolute pathnames, not relative pathnames.
SHARED_LIBRARY_R_LIST = -Wl,-R,$(PTOLEMY)/lib.$(PTARCH):$(PTOLEMY)/octtools/lib.$(ARCH):$(X11_LIBDIR):$(SHARED_COMPILERDIR):$(PTOLEMY)/tcltk/tcl.$(PTARCH)/lib/shared:$(PTOLEMY)/tcltk/tk.$(PTARCH)/lib/shared/:$(PTOLEMY)/tcltk/itcl.$(PTARCH)/lib/shared

# Shared libraries that libg++.so needs
SHARED_SYSLIBS =	-lio -lcurses -lrx 

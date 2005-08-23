# Template makefile used to compile C files and load them into Tcl.
#
# %W% %G%
# Makefile Author: Christopher Hylands
#
# Copyright (c) 1997 The Regents of the University of California.
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
##########################################################################

# Root of Tycho directory
# We use the TYCHO environment variable here, rather than a relative path
# so that we can more easily generate this file from within Tycho.
ROOT =		$(TYCHO)
# True source directory
#VPATH =		$(ROOT)/editors/textedit/template

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

OTHERCFLAGS =

include $(TYCHO)/obj.$(PTARCH)/java.mk
include $(TYCHO)/obj.$(PTARCH)/tcl.mk

# Tycho substitutes in our filename in the line below
BASENAME = myTclCmd

SRCS =	$(BASENAME).c
OBJS =	$(BASENAME).o
LIB =	$(BASENAME)$(TCL_SHLIB_SUFFIX)

EXTRA_SRCS =

MISC_FILES =

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =

INCLUDE = \
	-I./ \
	-I$(srcdir) \
	$(TCL_INCLUDES)


# Sub directories to run make in.
DIRS =

$LIBPATH = \
	-L$(TCL_LIB_DIR) \
	-L$(TK_LIB_DIR)

all: $(LIB) $(MISC_SRCS) 

install: $(LIBDIR)/$(LIB)

$(LIB) : $(OBJS)	
	$(TCL_SHLIB_LD) $(OBJS) -o $@ $(TCL_LD_SEARCH_FLAGS) $(LIBS)

depend:
	@echo "no dependencies in this directory"

# Get the rest of the rules
include $(ROOT)/mk/tycommon.mk

# Ptolemy makefile skeleton used by makefile_C CGC target
# Version:
# @(#)makefile_TclTk_VIS.mk	1.2 08/08/96
# Copyright (c) 1996 The Regents of the University of California.
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

ROOT =	$(PTOLEMY)
include $(ROOT)/mk/config-$(PTARCH).mk
C_INCL =

# We turn the optimizer off here, because -O2 conflicts with the default
# optimizer needed for the VIS code
OPTIMIZER = 

# redefine the compile to cc (not gcc)
CC = cc

#VIS include files and inline files
VISFLAGS = -fast -xO4 -xdepend -xchip=ultra -xarch=v8plusa -V
VIS_INCL = -I${VSDKHOME}/include ${VSDKHOME}/util/vis.il

# These two variables are shorthand for what is necessary to
# compile and link a tk application.
TKCFLAGS = $(X11_INCSPEC) -I$(TK_INCDIR) -I$(TCL_INCDIR) -I$(TK_INCDIR) \
	-I$(ITCL_INCDIR) -I$(ITK_INCDIR) \
	-I$(PTOLEMY)/src/domains/cgc/tcltk/lib
TKLOADLIBES = $(SHARED_LIBRARY_R_LIST) \
	-L$(PTOLEMY)/lib.$(PTARCH) $(TK_LIBSPEC) $(ITK_LIBSPEC) \
	$(TCL_LIBSPEC) $(ITCL_LIBSPEC) $(X11_LIBSPEC) $(CSYSLIBS)

# We are not assuming GNU make, so we can't include common.mk,
# so we have to set up our own .c.o rule
.c.o:
	$(CC) -c $(VISFLAGS) $(TKCFLAGS) $(VIS_INCL) $(OTHERCFLAGS) $<

# The GNU make info page says:
#  "`N' is made automatically from `N.o' by running the linker 
#  (usually called `ld') via the C compiler. The precise command
#  used is `$(CC) $(LDFLAGS) N.o $(LOADLIBES)'."

# Define a rule to build binaries from scratch.  Note that this
# is a GNU make extension
%: %.o
	$(LINK.o) $^ $(LOADLIBES) $(TKLOADLIBES) $(LDLIBS) -o $@
#$(LOADLIBES)
#$(LDLIBS)

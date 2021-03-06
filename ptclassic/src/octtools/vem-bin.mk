# Version Identification:
# @(#)vem-bin.mk	1.9	7/1/94
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
# common included makefile for vem binary subdirectories (src/vem/vem/*)
# built from a make.template file.

CC =	$(OCT_CC)

all::	makefile $(OBJS)

ifeq ($(USE_SHARED_LIBS),yes)
# Rule to install libdialogs and librpcserver for hppa
$(OCTLIBDIR)/$(LIB_INSTALL): $(LIB_INSTALL) 
		rm -f $@
		ln $^ $(OCTLIBDIR)
else
# Rule to install libdialogs and librpcserver for sol2, non-shared
$(OCTLIBDIR)/$(LIB_INSTALL): $(LIB_INSTALL) 
		rm -f $@
		ln $^ $(OCTLIBDIR)
		$(RANLIB) $@
endif
install: makefile $(OCTLIBDIR)/$(LIB_INSTALL)


C_INCL= -I../include -I../bitmaps -I../../include $(PTCOMPAT_INCSPEC) $(X11_INCSPEC)

lint:	$(LIBCSRCS)
	lint -u $(C_INCL) $^ > lint

#$(OBJS):
#	$(CC) $(C_SHAREDFLAGS) $(CFLAGS) $(OTHERCFLAGS) $(C_INCL) -c $<

# Fake out rules in common.mk for $(LIBDIR)/$(LIB) and 
# $(LIBDIR)/$(LIBNONSHARED)
LIB = 	dummylib
LIBNONSHARED =	dummylibnonshared

# Version Identification:
# @(#)vem-lib.mk	1.12	9/1/94
# Copyright (c) 1990-1994 The Regents of the University of California.
# All rights reserved.
# 
# Permission is hereby granted, without written agreement and without
# license or royalty fees, to use, copy, modify, and distribute this
# software and its documentation for any purpose, provided that the above
# copyright notice and the following two paragraphs appear in all copies
# of this software.
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
# common included makefile for all vem libraries (src/vem/Packages and 
# src/vem/Xpackages) built from a make.template file.

CC =	$(OCT_CC)
all:	makefile $(LIB) ../../include/${EXPHDR}

#clean::
#	rm -f *.a

# Don't place $(OCTTOOLS) and $(OCTTOOLS)/include as dependencies, or
# the header files we be installed each time the directories change
header:: ../../include/${EXPHDR} 

${OCTTOOLS}/include/${EXPHDR} ../../include/${EXPHDR}: $(EXPHDR)
	if [ ! -d $(OCTTOOLS) ]; then mkdir $(OCTTOOLS) ; fi
	if [ ! -d $(OCTTOOLS)/include ]; then mkdir $(OCTTOOLS)/include ; fi
	rm -f $@
	cp $< $@

$(OCTLIBDIR)/$(LIB):	$(LIB)
		rm -f $(OCTLIBDIR)/$(LIB)
		ln $(LIB) $(OCTLIBDIR)

install: makefile $(OCTLIBDIR)/$(LIB) ${OCTTOOLS}/include/${EXPHDR}

# Not all libraries have tests
tests::

C_INCL= -I../../include $(X11_INCSPEC)

lint:	$(LIBCSRCS)
	lint -C$(basename $(LIB) ) -u -I$(OCTTOOLS)/include $^ > lint
	lint -u -I$(OCTTOOLS)/include $^ > lint

LINTDIR=$(OCTTOOLS)/lib.$(ARCH)/lint

$(OCTTOOLS)/lib.$(ARCH)/lint:
	mkdir $@

install_lint: $(OCTTOOLS)/lib.$(ARCH)/lint
	cp llib-l$(basename $(LIB) ).ln $(LINTDIR)

$(DRIVER): $(DRVROBJ) $(OBJS) 
	$(LINKER) $(LINKFLAGS) $^ -o $@ -L$(OCTLIBDIR) $(DRVRLIBS)

# Local test driver scratch dir
# Needs to be phony so that make does not use vpath to find the directory
.PHONY:	$(DRVRDIR)
$(DRVRDIR):
	if [ ! -d $@ ]; then \
		echo "Creating $@"; \
		mkdir $@; (cd $(VPATH); tar -chf - $@) | tar -xpf -; \
	fi

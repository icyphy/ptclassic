# Version Identification:
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
# common included makefile for all vem libraries (src/vem/Packages and 
# src/vem/Xpackages) built from a make.template file.


# If we are building on the irix5, build octtools libraries as shared
# libraries.  If octtools is built as static libraries, then the GOT will
# overflow when linking a full blown pigiRpc.  See the Irix dso man page for
# more information

ifeq ($(PTARCH),irix5)
ifeq ($(USE_OCTTOOLS_SHARED_LIBS),yes)
IRIX5_SHL_MK=$(ROOT)/mk/octtools-irix5-shl.mk
include $(IRIX5_SHL_MK)
endif # USE_OCTTOOLS_SHARED_LIBS
endif

CC =	$(OCT_CC)
all:	makefile $(CLIB) ../../include/${EXPHDR}

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

$(OCTLIBDIR)/$(CLIB):	$(CLIB)
		rm -f $(OCTLIBDIR)/$(CLIB)
		ln $(CLIB) $(OCTLIBDIR)

install: makefile $(OCTLIBDIR)/$(CLIB) ${OCTTOOLS}/include/${EXPHDR}

# Not all libraries have tests
tests::

C_INCL= -I../../include $(PTCOMPAT_INCSPEC) $(X11_INCSPEC)

lint:	$(LIBCSRCS)
	lint -C$(basename $(CLIB) ) -u -I$(OCTTOOLS)/include $^ > lint
	lint -u -I$(OCTTOOLS)/include $^ > lint

LINTDIR=$(OCTTOOLS)/lib.$(PTARCH)/lint

$(OCTTOOLS)/lib.$(PTARCH)/lint:
	mkdir $@

install_lint: $(OCTTOOLS)/lib.$(PTARCH)/lint
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

# Fake out rules in common.mk for $(LIBDIR)/$(LIB) and 
# $(LIBDIR)/$(LIBNONSHARED)
LIB = 	dummylib
LIBNONSHARED =	dummylibnonshared

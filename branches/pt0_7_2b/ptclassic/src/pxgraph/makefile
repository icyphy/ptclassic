# Ptolemy-style makefile to create a link for the Java pxgraph script
#
# @(#)makefile	1.10 02/04/99
# Copyright (c) 1997-1999 The Regents of the University of California.
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

# Root of Ptolemy directory
ROOT=../..
# Where we are
VPATH = ../../src/pxgraph
# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

MISC_FILES = README.txt
all: sources
	@echo "There are two versions of pxgraph:"
	@echo "the older one that uses X11 sources"
	@echo "   $$PTOLEMY/src/pxgraph/pxgraph"
	@echo "the newer one that is implemented in Java "
	@echo "   $$PTOLEMY/tycho/java/ptolemy/plot"
	@echo " The default is to use the Java version."
	@echo " If you want to use the X11 version,"
	@echo " see $$PTOLEMY/src/pxgraph/README.txt"
	cd pxgraph; $(MAKE) CC="$(CC)" CFLAGS="$(CFLAGS)"

install: all $(BINDIR)/pxgraph.x11 $(BINDIR)/pxgraph 

$(BINDIR)/pxgraph: $(ROOT)/tycho/java/ptolemy/plot/pxgraph
	rm -f $@
	(cd $(BINDIR); ln -s ../tycho/java/ptolemy/plot/pxgraph .)

FORCE:
$(BINDIR)/pxgraph.x11: FORCE
	rm -f $@
	@if [ -f pxgraph/pxgraph$(BINARY_EXT) ]; then \
		echo "Now installing pxgraph/pxgraph.x11";\
		cp pxgraph/pxgraph$(BINARY_EXT) $(BINDIR)/pxgraph.x11;\
	else \
		echo "pxgraph/pxgraph$(BINARY_EXT) does not exist,";\
		echo "so we will use the Java version of pxgraph.";\
		echo "If you want to use the";\
		echo "X11 version of pxgraph, see";\
		echo "$$PTOLEMY/src/pxgraph/README.txt";\
	fi;

makefiles:
sources: makefile $(MISC_FILES)
	@if [ ! -d pxgraph ]; then echo "making pxgraph dir"; mkdir pxgraph; fi
	cd pxgraph; ../$(VPATH)/pxgraph/configure \
		--srcdir=../$(VPATH)/pxgraph

clean:
	cd pxgraph; $(MAKE) clean
depend:
	echo no dependencies to make in this directory

checkjunk:
	@checkextra -v makefile $(MISC_FILES) pxgraph SCCS

sccsinfo:
	@sccs check || true

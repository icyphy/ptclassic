# Ptolemy-style makefile to create a link for the Java pxgraph script
#
# $Id$
# Copyright (c) 1997-%Q% The Regents of the University of California.
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
all:
	@echo "Nothing to be done in this directory,"
	@echo " we now use the Java version of pxgraph"
	@echo " If you want to use the X11 version,"
	@echo " see $PTOLEMY/src/pxgraph/README.txt"

install: $(BINDIR)/pxgraph $(BINDIR)/pxgraph.x11

$(BINDIR)/pxgraph: $(ROOT)/tycho/java/ptplot/pxgraph
	rm -f $@
	(cd $(BINDIR); ln -s ../tycho/java/ptplot/pxgraph .)

FORCE:
$(BINDIR)/pxgraph.x11: FORCE
	rm -f $@
	@if [ -f pxgraph/pxgraph ]; then \
	       echo "Installing src/pxgraph/pxgraph as $(BINDIR)/pxgraph.x11";\
		(cd $(BINDIR); cp ../src/pxgraph/pxgraph/pxgraph \
			 pxgraph.x11); \
	else \
		echo "pxgraph/pxgraph does not exist, so we will use the";\
		echo "Java version of pxgraph.  If you want to use the";\
		echo "X11 version of pxgraph, see";\
		echo "$$PTOLEMY/src/pxgraph/README.txt";\
	fi;

makefiles:
sources: makefile $(MISC_FILES)
	(cd pxgraph; make sources)

clean:
depend:
	echo no dependencies to make in this directory

checkjunk:
	@checkextra -v makefile $(MISC_FILES) pxgraph SCCS

sccsinfo:
	@sccs check || true

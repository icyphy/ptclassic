# Ptolemy-style makefile to create a link for the Java pxgraph script
#
# $Id$
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

# Root of Ptolemy directory
ROOT=../..
# Where we are
VPATH = ../../src/pxgraph
# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

all:
	@echo "Nothing to be done in this directory,"
	@echo " we now use tycho/java/ptplot/pxgraph."
	@echo " For the old X11 version of pxgraph,"
	@echo " see ftp://ptolemy.eecs.berkeley.edu/pub/misc/ptplot"

install: $(BINDIR)/pxgraph

$(BINDIR)/pxgraph: $(ROOT)/tycho/java/ptplot/pxgraph
	rm -f $@
	(cd $(BINDIR); ln -s ../tycho/java/ptplot/pxgraph .)

makefiles:
sources: makefile

clean:
depend:
	echo no dependencies to make in this directory

checkjunk:
	@checkextra -v makefile SCCS

sccsinfo:
	@sccs check || true
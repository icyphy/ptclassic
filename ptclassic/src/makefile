# Makefile for all of Ptolemy source
#
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
# Programmer:  J. T. Buck
# $Id$
#
# Important!  Do not run this makefile while in ~ptolemy/src.  Instead
# run in from ~ptolemy/obj.$ARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

DIRS = kernel thread ptlang domains ptcl pigilib libgantt ptklib tycho pigiRpc pxgraph filters utils

# "leaf" directories with a make.template
MTDIRS = kernel pigilib libgantt pxgraph ptcl ptklib

# "leaf" directories with no make.template
LDIRS = ptlang pigiRpc filters utils

# directories with subtrees
SDIRS = thread domains

# directories to build TAGS tables in
TDIRS = kernel domains

MAKEVARS = "ARCH=$(ARCH)"

# This target makes sure all the makefiles are current
makefiles:
	@for x in $(MTDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making makefile in $$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../src/$$x makefile ;\
		) \
	    fi ; \
	done

	@for x in $(SDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ;\
		) \
	    fi ; \
	done

all install clean realclean sources depend checkjunk sccsinfo:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ;\
		) \
	    fi ; \
	done

.PHONY:	TAGS
TAGS:
	@for x in $(TDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../src/$$x $@ ; \
		) \
	    fi ; \
	done
	-cat -s $(TDIRS:%=%/TAGS) > $@

# Makefile for Ptolemy compatibility includes

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

# Important!  Do not run this makefile while in ~ptolemy/src/compat.  Instead
# run in from ~ptolemy/obj.$PTARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.


DIRS = X11R4 X11R5 cfront freebsd ptolemy
DEEPDIRS = matlab/extern/include mathematica/Source/Includes

# Need to have $PTOLEMY as an absolute pathname for wish
#PTOLEMY = ../..

MAKEVARS =	\
		"PTARCH=$(PTARCH)" 

all install clean realclean sources depend checkjunk sccsinfo:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		cd $$x ; \
		echo making $@ in compat/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/utils/$$x $@ ;\
	 	cd .. ; \
	    fi ; \
	done
	@for x in $(DEEPDIRS); do \
	    if [ -w $$x ] ; then \
		cd $$x ; \
		echo making $@ in utils/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../../../src/utils/$$x $@ ;\
	 	cd ../../.. ; \
	    fi ; \
	done

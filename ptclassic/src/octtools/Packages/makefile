# Version Identification:
# $Id$
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

# Makefile for octtools Packages.  Used to compile vem

# Important!  Do not run this makefile while in ~ptolemy/src/octtools/Packages.
# Instead run in from ~ptolemy/obj.$ARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

DIRS = avl cp da errtrap fang fc gu harpoon kd lel list nle oct octmm \
	oh options pep port pq region st symbolic symlib tap th timer \
	tr uprintf utility vov vulcan


# "leaf" directories with a make.template
MTDIRS = avl cp da errtrap fang fc gu harpoon kd lel list nle oct octmm \
	oh options pep port pq region st symbolic symlib tap th timer \
	tr uprintf utility vov vulcan

# "leaf" directories with no make.template
LDIRS = 

# directories with subtrees
SDIRS = 

MAKEVARS = "ARCH=$(ARCH)"

# This target makes sure all the makefiles are current
makefiles:
	@for x in $(MTDIRS); do \
		cd $$x ; \
		echo making makefile in $$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../../src/octtools/Packages/$$x makefile ; \
		cd .. ; \
	done

all install clean realclean sources depend checkjunk sccsinfo header \
tests lint:
	@for x in $(DIRS); do \
		cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../../src/octtools/Packages/$$x $@ ;\
		cd .. ; \
	done


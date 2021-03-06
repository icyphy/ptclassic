# Makefile for Ptolemy tkoct

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

# Important!  Do not run this makefile while in ~ptolemy/src/octtools/tkoct.
# Instead run in from ~ptolemy/obj.$PTARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

# tkoct was originally authored by Kennard White
# Ptolemy style makefile by Christopher Hylands (11/94)

# Note that tkoct and oct2ptcl use libraries from top.  tkoct uses libraries
# from tkgraph.  We don't usually build tkoct and tkgraph.
DIRS = 		top oct2ptcl #tkoct tkgraph

# Don't define PTOLEMY as a relative pathname or oct2ptcl will
# have an incorrect rpath and you won't be able to find the libraries
# at runtime
#PTOLEMY = 	../../..

MAKEVARS =	\
		"PTARCH=$(PTARCH)" 

TMPVPATH = ../../../../src/octtools/tkoct

makefiles:
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo Updating makefile in domains/$(ME)/$$x ; \
		  if [ ! -f $(TMPVPATH)/$$x/make.template \
			-a -f $(TMPVPATH)/$$x/makefile ]; then \
		     echo "make.template does not exist, linking make.template to makefile in $(TMPVPATH)/$$x"; \
		     (cd $(TMPVPATH)/$$x; ln -s makefile make.template); \
	          fi; \
		  $(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=$(TMPVPATH)/$$x $@ ; \
		) \
	    fi ; \
	done

all install clean sources depend realclean checkjunk sccsinfo:
	 @for x in $(DIRS); do \
	 	cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../../src/octtools/tkoct/$$x $@ ;\
	  	cd .. ; \
	 done



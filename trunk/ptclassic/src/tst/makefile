# Makefile to test domains
# Version identification:
# $Id$
# Copyright (c) %Q% The Regents of the University of California.
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
# Programmer: Christopher Hylands
# Date of creation: 5/24/96

# Ptolemy test bed
#
# 1. Run all the demos, look for core dumps
# 2. Check the results of the demos
# 3. Run purify while running the demos
#
# We use Kennard White's oct2ptcl program to translate oct facets to ptcl code.
# Note that some facets have glitches in them, so oct2ptcl barfs.

#DOMAINS = sdf hof de ddf bdf cg cgc cg56 vhdl vhdlb mdsdf $(EXPDOMAINS)
DOMAINS = bdf ddf de mdsdf sdf

MAKEVARS = "PTARCH=$(PTARCH)"

# Directories we want to run in
DIRS = 	$(DOMAINS) incLink

sources test all.pt diff updateresults clean realclean::
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in tst/$$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
		) \
	    fi ; \
	done

# Use this to create makefiles from scratch.  Most users won't have to do
# this, as the makefiles should have been edited by hand to exclude 
# palettes that won't work with oct2ptcl.
makefiles:
	@for x in $(DOMAINS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		if [ ! -f makefile ]; then \
			echo "Creating $$x/makefile"; \
			$(PTOLEMY)/src/tst/genmakefiletst $$x > makefile ; \
		fi ; \
		) \
	    fi ; \
	done

VPATH=../../src/tst


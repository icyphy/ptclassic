# Top-level Ptolemy makefile.
# Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
#		       
# Programmer:  J. T. Buck
#
# The ARCH environment variable must be set.
# $Id$

defaultTarget:		install
	@echo Installation for $(ARCH) is complete.

makefiles:		obj.$(ARCH)
	@echo Updating makefiles.  Ignore messages about needing to rerun make --
	@echo the top-level makefile will make sure it is done right.
	cd obj.$(ARCH); $(MAKE) $(MFLAGS) makefiles

all install:	sources obj.$(ARCH)
	cd obj.$(ARCH); $(MAKE) $(MFLAGS) $@

clean:
	cd obj.$(ARCH); $(MAKE) $(MFLAGS) clean

sources depend:
	cd src; $(MAKE) $(MFLAGS) makefiles $@

obj.$(ARCH):		MAKEARCH
	csh -f MAKEARCH

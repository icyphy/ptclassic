# Makefile included at the root of each domain (eg. SDF, DE, etc) for tycho
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

# This file is used by tycho, see $PTOLEMY/mk/topdomain.mk for the file
# use by ptolemy.

# Based on topdomain.mk by Joe Buck.  The differences between this file
# and topdomain.mk:
# 	The VPATH is different
#	This file does not use make.template

MAKEVARS = "PTARCH=$(PTARCH)"

makefiles depend all install clean sources itcldocs \
	realclean checkjunk sccsinfo extraclean tests weblint :
	@for x in $(DIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		  echo making $@ in domains/$(ME)/$$x ; \
		  $(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../tycho/domains/$(ME)/$$x $@ ; \
		) \
	    fi ; \
	done

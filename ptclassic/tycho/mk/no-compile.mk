# common include file for directories that don't get compiled and don't
# need dependencies generated
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


# This makefile is to be included if we don't need the compiler
# and we don't need to generate any dependencies

all install TAGS:
	@echo "Nothing to be done in this directory"

# "make sources" will do SCCS get on anything where SCCS file is newer.
sources:	$(EXTRA_SRCS) $(SRCS) $(HDRS) makefile

CRUD=*.o *.so core *~ *.bak ,* LOG* $(KRUFT) 

clean:
	rm -f $(CRUD)

depend:
	@echo "no dependencies in this directory"

sccsinfo:
	sccs info

makefiles: makefile


# You probably don't want to add $(SRCS) here, since really $(SRCS)
# get compiled and have dependencies.  Instead, modify the makefile
# that includes this one and have it set $(EXTRA_SRCS)
checkjunk:
	@checkextra -v $(HDRS) $(EXTRA_SRCS) $(MISC_FILES) makefile SCCS


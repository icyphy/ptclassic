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
# and we don't need to generate any dependencies.  Please don't use
# GNU make extensions in this file, such as 'ifdef'.

all install TAGS: $(EXTRA_SRCS) $(HDRS) $(MISC_FILES)
	@echo "Nothing to be done in this directory"

# "make sources" will do SCCS get on anything where SCCS file is newer.
# You probably don't want to add $(SRCS) to this, rather, change
# the makefile that includes this one to use $(EXTRA_SRCS)
sources::	$(EXTRA_SRCS) $(HDRS) $(MISC_FILES) makefile
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

depend:
	@echo "no dependencies in this directory"

sccsinfo:
	sccs info
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

makefiles: makefile
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

# Generate html files from itcl files, requires itclsh and tycho
# Note that $(ROOT) here is relative to the tycho directory, not
# the Ptolemy directory.
itcldocs:
	@if [ "$(TYDOC_DESC)" = "" ] ; then \
		echo "$(ROOT)/lib/tydoc/tydoc $(ITCL_SRCS)"; \
	 	$(ROOT)/lib/tydoc/tydoc $(ITCL_SRCS); \
	else \
		echo "$(ROOT)/lib/tydoc/tydoc -t "$(TYDOC_DESC)" $(ITCL_SRCS)"; \
		$(ROOT)/lib/tydoc/tydoc -t "$(TYDOC_DESC)" $(ITCL_SRCS); \
	fi

# You probably don't want to add $(SRCS) here, since really $(SRCS)
# get compiled and have dependencies.  Instead, modify the makefile
# that includes this one and have it set $(EXTRA_SRCS)
checkjunk:
	@checkextra -v $(HDRS) $(EXTRA_SRCS) $(MISC_FILES) makefile SCCS
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

CRUD=*.o *.so core *~ *.bak ,* LOG* $(KRUFT) 

clean:
	rm -f $(CRUD)
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi

realclean:
	rm -f $(CRUD) $(REALCLEAN_STUFF)
	rm -rf doc/codeDoc/*
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	fi



# Remove the sources too, so that we can get them back from sccs
extraclean:
	rm -f $(CRUD) $(REALCLEAN_STUFF) $(EXTRA_SRCS)
	@if [ "x$(DIRS)" != "x" ]; then \
		set $(DIRS); \
		for x; do \
		    if [ -w $$x ] ; then \
			( cd $$x ; \
			echo making $@ in $$x ; \
			$(MAKE) $(MFLAGS) $(MAKEVARS) $@ ;\
			) \
		    fi ; \
		done ; \
	if

# Create tclIndex from .tcl and .itcl files
# This rule must be after the TCL_SRC and ITCL_SRC lines in the makefile
# that includes this makefile
tclIndex: $(TCL_SRCS) $(ITCL_SRCS)
	@echo "Updating tclIndex"
	rm -f $@
	echo 'set auto_path [linsert $$auto_path 0 [info library] ]; auto_mkindex . $(TCL_SRCS) $(ITCL_SRCS)' | $(ITCLSH)


# Makefile common to all domain test makefiles
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
# Date of creation: 4/24/95

# The makefiles in the domain tests include two makefiles, this file should
# be included at the top of the makefile for the domain tests.

# If you want to run with purify, do:
# make USE_PURIFY=yes

# Note that the ifeq is a GNU make extension.
ifeq ($(USE_PURIFY),yes)
# Name of purified ptcl binary to use
PTCL= $(PTOLEMY)/obj.$(PTARCH)/ptcl/ptcl.debug.purify
else
# Name of ptcl binary to run
PTCL= $(PTOLEMY)/bin.$(PTARCH)/ptcl
endif

SRC = $(PTOLEMY)/src

# Clean up the .pt file.  Most of these problems have to do with
# 'targetparam' in cgc
FIXPT4TEST= sed -e 's=\$$PTOLEMY/bin.\$$PTARCH/==' \
	-e 's=\$$HOME/PTOLEMY_SYSTEMS=PTOLEMY_SYSTEMS=' \
	-e 's=destDirectory=directory=' \
	-e 's=\{Destination Directory\}=directory=' \
	-e 's=hostMachine=host=' \
	-e 's=loopingLevel=\{Looping Level\}=' \
	-e 's=doCompile=compile?=' \
	-e 's=^wrapup=wrapup; flush stdout='

OCT2PTCL = /users/cxh/pt/bin.sol2.5/oct2ptcl

#usage: /users/ptdesign/obj.sol2/octtools/tkoct/oct2ptcl/oct2ptcl [-=V]
#                    [-=E on_error] [-rgtv] facet
#   -=V:        disable tracing with design manager VOV
#   -=E:        cause fatal errors to core dump (on_error = "core") or exit
#               (on_error = "exit")
#   -r:         recursive translation
#   -g:         append run/wrapup command (go)
#   -t:         don't xlate facets with tcltk stars
#   -v:         verbose
#   facet:      facet to translate
#oct2ptcl v0.1 translates an OCT facet to ptcl.
#The PTOLEMY environment variable must reference the root
#of the ptolemy tree, and defaults to ~ptolemy.


.PRECIOUS: %.pt
# Convert demo palettes to ptcl files.
%.pt: %.pal
	-if [ -f $(TST_DOMAIN)/`basename $@ .pt`.base ]; then \
		echo "found $@.base, using it instead of running oct2ptcl"; \
		cp $(TST_DOMAIN)/`basename $@ .pt`.base $@; \
		echo "running oct2ptcl to see what the diffs are"; \
		$(OCT2PTCL) -rgv $< | $(FIXPT4TEST) >  /tmp/pttst.tmp; \
		diff /tmp/pttst.tmp $@; \
	else \
		$(OCT2PTCL) -rgv $< | $(FIXPT4TEST) >  $(notdir $@); \
	fi

# Run files containing ptcl commands through ptcl, running the universes,
# producing output and data files.
# *.pxgraph contains a script that will plot the pxgraphs
#  we sort this script since the order that the pxgraphs are run is 
#  non-deterministic	
%.pout: %.pt
	echo "$^" >pxfilename
	rm -f $^.pxgraph
	(PATH=$$PTOLEMY/src/tst:$$PATH; export PATH; \
		PT_DISPLAY="echo %s"; export PT_DISPLAY; \
		echo "source $(TST_PREAMBLE); source $^" | $(PTCL) | tee $@)
	sort $^.pxgraph > $^.pxgraph.tmp; mv $^.pxgraph.tmp $^.pxgraph
# End of implicit rules


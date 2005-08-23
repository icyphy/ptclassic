# Makefile that creates idx files
# @(#)domain.mk	1.9 10/04/96
# Copyright (c) 1996 The Regents of the University of California.
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
# Date of creation: 9/25/96
# Written by: Christopher Hylands
#
# .idx files are Index files used by the tycho system
# This makefile is usually included at the bottom of domains/foo/makefile
#

ifndef SUBSTARIDXS
# starHTML.idx files in the subdomain directories, like sdf/dsp
SUBSTARIDXS =	$(addprefix $(VPATH)/, \
			$(addsuffix /stars/starHTML.idx, $(SUBDOMAINDIRS)))

# All of the starHTML.idx files to go into the domain starHTML.idx file
ALLSTARIDXS = 		$(SUBSTARIDXS) $(VPATH)/stars/starHTML.idx
endif

# Generic rule to produce a starHTML.idx file.  This rule depends
# on all the .htm files in the directory where the starHTML.idx file
# is to be built
$(ALLSTARIDXS): $(wildcard $(dir $@)/*.htm) $(TYCHOMAKEINDEX)
	@echo "Updating $@";
	@rm -f $@;
	@echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(TYCHOMAKEINDEX); \
		ptolemyStarHTMLIndex $(ME) \
			$@  \
			$(wildcard $(dir $@)*.htm) " \
		| itclsh

# Star index of all the star index files
starHTML.idx: $(ALLSTARIDXS) $(TYCHOMAKEINDEX)
	@echo "Merging $(VPATH)/$@"
	@rm -f $(VPATH)/$@
	@(cd $(VPATH); \
	 echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(TYCHOMAKEINDEX); \
		tychoVpathMergeIndices \"$(ME) stars\" \
			 $(VPATH)/$@ $(VPATH)/ $(ALLSTARIDXS)" | itclsh)



# star/demo recursive index
STARDEMOIDXS = 	$(VPATH)/starDemo.idx

$(STARDEMOIDXS) demo.idx: $(wildcard $(dir $@)/schematic/contents\;) \
		$(STARINDEX)
	@echo "Updating $@"
	@(cd $(VPATH); \
	 echo "source $(STARINDEX); \
		starindex_WriteDemoIndex $(ME) " | itclsh)

domain.idx: $(STARDEMOIDXS) starHTML.idx $(TYCHOMAKEINDEX)
	@echo "Merging $(VPATH)/$@"
	@rm -f $(VPATH)/$@
	@(cd $(VPATH); \
	 echo "set TYCHO $(PTOLEMY)/tycho; \
		source $(TYCHOMAKEINDEX); \
		tychoVpathMergeIndices \"$(ME) stars and demos\" \
			$(VPATH)/$@ $(VPATH)/ \
			$(STARDEMOIDXS) starHTML.idx demo.idx \
			 " | itclsh)

clean_indices:
	rm -f $(ALLSTARIDXS) starHTML.idx starHTML.idx.fst $(STARDEMOIDXS)

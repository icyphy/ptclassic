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
# Makefile for vem source

# Important!  Do not run this makefile while in ~ptolemy/src/octtools.  Instead
# run in from ~ptolemy/obj.$ARCH (through the symbolic link) -- that
# way a separate directory containing, say, Sun-3 and Sun-4 objects can be
# maintained.

DIRS = Packages Xpackages vem

LIBDIRS = Packages Xpackages

# "leaf" directories with a make.template
MTDIRS = attache installColors masters oct2ps octfix octls


# "leaf" directories with no make.template
LDIRS =

# All directories
ALLDIRS = $(DIRS) $(MTDIRS) 

# Directories other than vem
OTHERDIRS = tkoct $(MTDIRS)

# directories with subtrees
SDIRS = Packages Xpackages vem tkoct

MAKEVARS = "ARCH=$(ARCH)"

OCTTOOLS=$(PTOLEMY)/octtools

.PHONY:	header liball libinstall vemall installvem otherinstall
all vem: header liball libinstall otherall vemall
install: header liball libinstall vemall installvem otherinstall

otherall:
	for x in $(OTHERDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo make all in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/octtools/$$x all ;\
		) \
	    fi ; \
	done

otherinstall: $(OCTTOOLS) $(OCTTOOLS)/lib.$(ARCH) $(OCTTOOLS)/include
	for x in $(OTHERDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo make install in $$x  ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/octtools/$$x install ;\
		) \
	    fi ; \
	done

# This target makes sure all the makefiles are current
makefiles:
	@for x in $(MTDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making makefile in $$x ; \
		$(MAKE) -f make.template $(MFLAGS) $(MAKEVARS) \
			VPATH=../../../src/octtools/$$x makefile ;\
		) \
	    fi ; \
	done

	@for x in $(SDIRS); do \
	    if [ -w $$x ] ; then \
		( cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/octtools/$$x $@; \
		) \
	    fi ; \
	done

header: $(OCTTOOLS) include $(OCTTOOLS)/include
	@for x in $(LIBDIRS); do \
		cd $$x ; \
		echo making header in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/octtools/$$x header; \
		cd .. ; \
	done
liball: 
	@for x in $(LIBDIRS); do \
		cd $$x ; \
		echo making all in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/octtools/$$x all;\
		cd .. ; \
	done
libinstall: $(OCTTOOLS) $(OCTTOOLS)/lib $(OCTTOOLS)/lib.$(ARCH) $(OCTTOOLS)/include
	@for x in $(LIBDIRS); do \
		cd $$x ; \
		echo making install in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/octtools/$$x install;\
		cd .. ; \
	done
vemall: 
	@echo "making all in vem"
	cd vem; \
	$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/octtools/vem all;\
	cd ..

installvem: $(OCTTOOLS) $(OCTTOOLS)/lib.$(ARCH) $(OCTTOOLS)/include
	@echo "making install in vem"
	cd vem; \
	$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/octtools/vem install;\
	cd ..

$(OCTTOOLS) $(OCTTOOLS)/include $(OCTTOOLS)/lib $(OCTTOOLS)/lib.$(ARCH):
	-mkdir $@

clean realclean sources depend checkjunk sccsinfo lint tests:
	@for x in $(ALLDIRS); do \
		cd $$x ; \
		echo making $@ in $$x ; \
		$(MAKE) $(MFLAGS) $(MAKEVARS) VPATH=../../../src/octtools/$$x $@ ;\
		cd .. ; \
	done

# common included makefile for all directories in which a makefile is
# built from a make.template file.
# Version identification:
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
# Date of creation: 6/15/92.
# Written by: J. Buck
#
# The following symbols are used by this makefile.  They should be
# defined by the parent makefile prior to including this makefile:
# SRCS
# OBJS
# LIB
# DOMAIN, DOMAIN_2, DOMAIN_3
# STAR_MK	Specifies the name of the star-reference file to build;
#		both a C-verion (e.g., cgcstars.c) using genStarTable and
#		a makefile version (e.g., cgcstars.mk) using genStarList 
#		is built.  The variable "STAR_MK" should be given without
#		a file suffix (e.g., just "cgcstars").
# GENSTARVARS	A variable passed to genStarTable to allow stars other
#		than those defined by PL_SRCS to be generated.

# This file is used when there are files to be compiled.  Usually a 
# make.template file is present if there are files to be compiled.
# See no-compile.mk for a similar file that is to be used when there
# are no files to be compiled.

whatToBuild:	all

.SUFFIXES:	.o .cc .h .pl .chdl .is

# Rule for compiling C++ files
.cc.o:
	$(CPLUSPLUS) $(CC_SHAREDFLAGS) $(GPPFLAGS) -I$(VPATH) $(INCL) -c $<

# Rule for compiling with cc
.c.o:
	$(CC) $(C_SHAREDFLAGS) $(CFLAGS) $(C_INCL) -c $<

# The optimizer in g++-2.7.2 has a bug that we workaround by turning
# off the optimizer.  The problem is that when the optimizer is turned on, 
# certain files end up needing  operator= or a copy constructor from 
# a parent class.  Unfortunately, the parent class does not define what
# is needed
#
# The following files use these two rules:
#  de/tcltk/stars/make.template
#  de/stars/make.template
#  cg56/stars/make.template
#  cp/stars/make.template (cp domain is present only on sun4)
#  cp/infopad/stars/make.template (cp domain is present only on sun4)
#  sdf/matlab/stars/make.template
#
# We include this rule in common.mk rather than config-g++.mk so that
# we can use any architecture specific variables in the
# UNOPTIMIZED_COMPILE_RULE.  Having this rule in config-g++.mk means
# that we don't see architecture dependent variables.
#
UNOPTIMIZED_WARNING_MESSAGE = @echo "DANGER: gcc-2.7.2 optimizer workaround here, see $$PTOLEMY/mk/config-g++.mk"

UNOPTIMIZED_COMPILE_RULE = 	$(CPLUSPLUS) $(CC_SHAREDFLAGS) $(WARNINGS) \
					$(ARCHFLAGS) $(LOCALCCFLAGS) \
					$(USERFLAGS) -I$(VPATH) $(INCL) -c 

# Note that forcing the installation of ptlang might not be the best
# thing to do, it would be best if 'make sources' did not touch the
# bin.$(PTARCH) directory, so we check to see if there is a ptlang in the
# obj.$(PTARCH)/ptlang directory and use it.  This is awkward, but the
# GNU tools do something similar

# ptlang binary in the obj directory
PTLANG_OBJ_DIR=$(PTOLEMY)/obj.$(PTARCH)/ptlang
PTLANG_IN_OBJ=$(PTLANG_OBJ_DIR)/ptlang
PTLANG_VPATH=../../src/ptlang

# Use either the ptlang binary in the obj directory or just use ptlang
PTLANG= `if [ -f $(PTLANG_IN_OBJ) ]; \
	then echo $(PTLANG_IN_OBJ) ; \
	else echo ptlang; fi`

# Build the ptlang binary if necessary
$(PTLANG_IN_OBJ):
	(cd $(PTLANG_OBJ_DIR); $(MAKE) VPATH=$(PTLANG_VPATH))

# islang binary in the obj directory
ISLANG_OBJ_DIR=$(PTOLEMY)/obj.$(PTARCH)/domains/ipus/icp/islang
ISLANG_IN_OBJ=$(ISLANG_OBJ_DIR)/islang
ISLANG_VPATH=../../../../../src/domains/ipus/icp/islang

# Use islang on the path; if it has not been installed yet, then install it
ISLANG= `if [ ! -f $(ISLANG_IN_OBJ) ]; \
	 then (cd $(ISLANG_OBJ_DIR) ; \
		  $(MAKE) $(ISLANG_IN_OBJ) VPATH=$(ISLANG_VPATH) >/dev/null 2>&1); fi; \
	 if [ ! -f $(ISLANG_IN_OBJ) ]; \
	 then echo islang; \
	 else echo $(ISLANG_IN_OBJ); fi`

# Build the islang binary if necessary
$(ISLANG_IN_OBJ):
	(cd $(ISLANG_OBJ_DIR); $(MAKE) VPATH=$(ISLANG_VPATH))

# pepp binary in the obj directory.  Pepp is used by Thor to convert
# .chdl files to .cc files.  'make sources' may need pepp
PEPP_OBJ_DIR=$(PTOLEMY)/obj.$(PTARCH)/domains/thor/pepp
PEPP_IN_OBJ=$(PEPP_OBJ_DIR)/pepp
PEPP_VPATH=../../../../src/domains/thor/pepp

# Use either the pepp binary in the obj directory or just use pepp
# I don't know why we can't use a rule like the ones for ptlang
# but it just seems to be a bug in gmake.  So instead, we run
# make by hand if necessary
PEPP= `if [ ! -f $(PEPP_IN_OBJ) ]; \
	then $(MAKE) $(PEPP_IN_OBJ) >/dev/null 2>&1; fi; \
	if [ ! -f $(PEPP_IN_OBJ) ]; \
	then echo pepp; \
	else echo $(PEPP_IN_OBJ); fi`

# Build the pepp binary if necessary
$(PEPP_IN_OBJ):
	(cd $(PEPP_OBJ_DIR); $(MAKE) VPATH=$(PEPP_VPATH) pepp)

# Rule to build the ../doc/star directory
# Can't use mkdir -p here, it might not exist everywhere
# Run 'exit 0' as the last command if the directories already exist.
#  (Otherwise the mips PTARCH will produce make error message like:
#  'make: *** [DETest.cc] Error 2' because the if [] statement is returning
#  non-zero if the directory exists.  sigh.)

$(STARDOCDIR):
	$(STARDOCRULE) 
STARDOCRULE=if [ ! -d `dirname $(STARDOCDIR)` ]; then \
		echo "Making directory `dirname $(STARDOCDIR)`"; \
		mkdir `dirname $(STARDOCDIR)`; \
	fi; \
	if [ ! -d $(STARDOCDIR) ]; then \
		echo "Making directory $(STARDOCDIR)"; \
		mkdir $(STARDOCDIR); \
	else exit 0; \
	fi

# Rules for running the ptlang processor
# Make sure we always run the preprocessor in the source directory
# the "mv" part moves the documentation to the doc dir.
# note if there is no doc dir, the command continues despite the error.
.pl.cc: $(PTLANG_IN_OBJ) $(STARDOCDIR)
	cd $(VPATH); $(PTLANG) $< 
	@$(STARDOCRULE)
	-if [ -w $(STARDOCDIR) ] ; then \
		 ( cd $(VPATH); mv -f $*.t $(STARDOCDIR)/. ) \
	fi ;

.pl.h: $(PTLANG_IN_OBJ) $(STARDOCDIR)
	cd $(VPATH); $(PTLANG) $< 
	@$(STARDOCRULE)
	-if [ -w $(STARDOCDIR) ] ; then \
		( cd $(VPATH); mv -f $*.t $(STARDOCDIR)/. ) \
	fi ;

# Rules for running the islang processor
# Make sure we always run the preprocessor in the source directory
# the "mv" part moves the documentation to the doc dir.
# note if there is no doc dir, the command continues despite the error.
.is.cc: $(ISLANG_IN_OBJ) $(STARDOCDIR)
	cd $(VPATH); $(ISLANG) $<
	@$(STARDOCRULE)
	-cd $(VPATH)

.is.h: $(ISLANG_IN_OBJ) $(STARDOCDIR)
	cd $(VPATH); $(ISLANG) $<
	@$(STARDOCRULE)
	-cd $(VPATH); mv $*.t $(STARDOCDIR)/.

# Rule for the thor preprocessor
# Make sure we always run the preprocessor in the source directory
.chdl.cc: $(PEPP_IN_OBJ)
	cd $(VPATH); $(PEPP) $<

ifeq ($(strip $(LIB)),)
LIB=dummylib
endif

# Rule for building a C++ library
# We use a GNU make conditional here
$(LIB):	$(OBJS)
ifeq ($(USE_SHARED_LIBS),yes) 
	rm -f $@
	$(SHARED_LIBRARY_COMMAND) $@ $(OBJS) 
	# HPUX wants shared libraries to be 555 for performance reasons
	chmod 555 $@
else
	rm -f $@
	$(AR) cq $@ $(OBJS)
	$(RANLIB) $@
endif

ifeq ($(strip $(CLIB)),)
CLIB=dummyclib
endif

# Rule for building a C library (no C++).  Used primarily by octtools
# We use a GNU make conditional here
$(CLIB):	$(OBJS)
ifeq ($(USE_SHARED_LIBS),yes) 
	rm -f $@
	$(CSHARED_LIBRARY_COMMAND) $@ $(OBJS) 
	# HPUX wants shared libraries to be 555 for performance reasons
	chmod 555 $@
else
	rm -f $@
	$(AR) cq $@ $(OBJS)
	$(RANLIB) $@
endif

# Used to explicitly build non-shared libraries. For an example, see 
# $PTOLEMY/src/domains/cgc/rtlib/make.template
$(LIBNONSHARED): $(OBJS)
	rm -f $@
	$(AR) cq $@ $^
	$(RANLIB) $@

$(LIBDIR)/$(LIBNONSHARED):	$(LIBNONSHARED) $(EXP)
	rm -f $@
	ln $(LIBNONSHARED) $(LIBDIR)

# AIX used EXP for export lists
$(EXP): $(LIB)

# Rule for installing a C++ library
$(LIBDIR)/$(LIB):	$(LIB) $(EXP)
		rm -f $@
		ln $(LIB) $(LIBDIR)

# Rule for installing a C library
$(LIBDIR)/$(CLIB):	$(CLIB) $(EXP)
		rm -f $@
		ln $(CLIB) $(LIBDIR)

# Rule for making a star list for inclusion by make
$(VPATH)/$(STAR_MK).mk:	make.template
		cd $(VPATH); rm -f $(STAR_MK).mk
		cd $(VPATH); genStarList $(DOMAIN) $(DOMAIN_2) $(DOMAIN_3) > $(STAR_MK).mk

# Rule for making a star list module for pulling stars out of libraries
$(VPATH)/$(STAR_MK).c:	make.template
		cd $(VPATH); rm -f $(STAR_MK).c
		cd $(VPATH); genStarTable $(MAKE) $(GENSTARVARS) $(DOMAIN) $(DOMAIN_2) $(DOMAIN_3) > $(STAR_MK).c

$(STAR_MK).o:	$(VPATH)/$(STAR_MK).c

# Standard "all" for stars directories
stars_all:	$(SRCS) $(LIB) $(STAR_MK).o

# Rule for installing both the star library and star list
stars_install:	all $(LIBDIR)/$(LIB) $(LIBDIR)/$(STAR_MK).o
		@echo Installation complete.

# Rule for installing the star list

$(LIBDIR)/$(STAR_MK).o:	$(STAR_MK).o
		rm -f $@
		ln $(STAR_MK).o $(LIBDIR)

# "make sources" will do SCCS get on anything where SCCS file is newer.
# Don't place $(STARDOCDIR) here, or the STARDOCDIR directory will be made
# in non-star related directories.
sources:	$(PTLANG_IN_OBJ) $(EXTRA_SRCS) $(SRCS) $(HDRS) make.template 
CRUD=*.o *.so *.sl core *~ *.bak ,* LOG* $(KRUFT) 
clean:
	rm -f $(CRUD)

# Make things "even cleaner".  Removes libraries, generated .cc and .h
# files from preprocessor, etc.
realclean:
	rm -f $(CRUD) $(LIB) $(CLIB) $(PL_SRCS:.pl=.h) $(PL_SRCS:.pl=.cc) $(REALCLEAN_STUFF)

DEPEND_INCL=$(INCL) $(C_INCL) $(SYSTEM_DEPEND_INCL)

depend:		$(SRCS) $(HDRS)
# We use a GNU make conditional here
ifeq ($(USE_MAKEDEPEND),yes) 
		# HPPA CC does not understand the -M option
		cd $(VPATH); $(DEPEND) $(DEPEND_INCL) $(SRCS)
else
		cd $(VPATH); \
                $(DEPEND) $(DEPEND_INCL) $(SRCS)|\
                        cat make.template - > makefile.n;\
                mv -f makefile.n makefile
endif

makefile:	make.template $(MDEPS)
		cd $(VPATH); rm -f makefile; cp make.template makefile; \
		$(MAKE) -f make.template sources depend
		if [ ! -f makefile ]; then ln -s $(VPATH)/makefile makefile; fi
		@echo makefile remade -- you must rerun make.
		exit 1

makefiles:	makefile

# Convert relative VPATH to an absolute path.
TAGS:		$(HDRS) $(SRCS)
		etags++ -b $(HDRS:%=$(VPATH:$(ROOT)%=$(PTOLEMY)%)/%) \
			$(SRCS:%=$(VPATH:$(ROOT)%=$(PTOLEMY)%)/%)

# Rule for detecting junk files
checkjunk:
	@checkextra -v $(SRCS) $(HDRS) $(EXTRA_SRCS) $(OTHERSRCS) \
		$(OBJS) $(LIB) \
		$(STAR_MK).o $(STAR_MK).mk $(EXTRA_DESTS) \
		$(MISC_FILES) makefile make.template SCCS TAGS

# "check" does not print anything if nothing is being edited.
sccsinfo:
	@sccs check || true

# Matlab settings
include $(ROOT)/mk/matlab.mk

# Mathematica settings
include $(ROOT)/mk/mathematica.mk

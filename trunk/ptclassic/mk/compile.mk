# supporting makefile for the common included makefile for all directories
# in which a makefile is built from a make.template file.
#
# Version identification:
# $Id$
#
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

ifeq ($(strip $(LIB)),)
LIB=dummylib
endif

ifeq ($(strip $(LIB_DEBUG)),)
LIB_DEBUG=dummydebuglib
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

STARHTMS = $(PL_SRCS:.pl=.htm)
starhtms: $(STARHTMS)

# "make sources" will do SCCS get on anything where SCCS file is newer.
sources:	$(PTLANG_IN_OBJ) $(EXTRA_SRCS) $(SRCS) $(HDRS) make.template \
			 $(STARHTMS)

CRUD=*.o *.so *.sl core *~ *.bak ,* LOG* $(KRUFT) 
clean:
	rm -f $(CRUD)

# Make things "even cleaner".  Removes libraries, generated .cc and .h
# files from preprocessor, etc.
realclean:
	rm -f $(CRUD) $(LIB) $(CLIB) \
		$(PL_SRCS:.pl=.h) $(PL_SRCS:.pl=.cc) \
		$(PL_SRCS:.pl=.htm) \
		TAGS starHTML.idx starHTML.idx.fst\
		$(REALCLEAN_STUFF)

# Remove the sources too, so that we can get them back from sccs
extraclean: realclean
	@if [ -d SCCS ]; then \
		echo "SCCS dir present, removing sources"; \
		rm -f $(PL_SRCS) $(SRCS) $(HDRS) $(SCRIPTS); \
	else \
		echo "SCCS dir not present, _not_ removing sources"; \
	fi

# The depend, makefile and makefiles rules are duplicated in
# src/pitcl/make.template, so if you make changes below, please
# change that file too.
DEPEND_INCL=$(INCL) $(C_INCL) $(SYSTEM_DEPEND_INCL)

depend:		$(SRCS) $(HDRS)
# We use a GNU make conditional here
ifeq ($(USE_MAKEDEPEND),yes) 
		# HPPA CC and SunSoft CC do not understand the -M option
		# so we use makedepend
		#
		# Note that makedepend does not produce dependencies
		# as GNU g++ -M, so you will probably want to run
		# 'make depend' from an PTARCH that uses
		# g++ -M instead of makedepend.  One way to do this
		# would be to do (Assuming you have g++ installed):
		# 	cd $PTOLEMY; make PTARCH=sol2 depend
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

# Command used in "make TAGS"
#
# Override with something like
#  gmake TAGS ETAGS="echo >>/tmp/allfiles"
# to generate a line of all source and header files
ETAGS =		etags++ -b

# Convert relative VPATH to an absolute path.
TAGS:		$(HDRS) $(SRCS)
		$(ETAGS) $(HDRS:%=$(VPATH:$(ROOT)%=$(PTOLEMY)%)/%) \
			$(SRCS:%=$(VPATH:$(ROOT)%=$(PTOLEMY)%)/%)

# Rule for detecting junk files
checkjunk:
	@checkextra -v $(SRCS) $(HDRS) $(EXTRA_SRCS) $(OTHERSRCS) \
		$(OBJS) $(LIB) $(PL_SRCS:.pl=.htm) \
		$(STAR_MK).o $(STAR_MK).mk $(EXTRA_DESTS) \
		$(MISC_FILES) makefile make.template SCCS TAGS

# Check out the star documentation for bogus html
# weblint is not shipped with Ptolemy, you can get it from
# http://www.khoros.unm.edu/staff/neilb/weblint.html
# weblint uses perl
weblint:
	weblint  -x Netscape *.htm* | grep -v 'empty container element <[AP]>'


# "check" does not print anything if nothing is being edited.
sccsinfo:
	@sccs check || true


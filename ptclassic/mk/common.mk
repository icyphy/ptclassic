# common included makefile for all directories in which a makefile is
# built from a make.template file.
# Version identification:
# $Id$
# Copyright (c) 1992 The Regents of the University of California.
#                       All Rights Reserved.
# Date of creation: 6/15/92.
# Written by: J. Buck
#
# This file should be included AFTER other symbols are defined,
# particularly SRCS, OBJS, LIB, DOMAIN, STAR_MK.

whatToBuild:	all

.SUFFIXES:	.o .cc .h .pl .chdl

# Rule for compiling C++ files
.cc.o:
	$(GNU) $(GPPFLAGS) -I$(VPATH) $(INCL) -c $<

# Rule for compiling with cc
.c.o:
	$(CC) $(CFLAGS) $(C_INCL) -c $<

# Rules for running the ptlang processor
# Make sure we always run the preprocessor in the source directory
.pl.cc:
	cd $(VPATH); ptlang $< ; mv $*.t $(STARDOCDIR)

.pl.h:
	cd $(VPATH); ptlang $< ; mv $*.t $(STARDOCDIR)

# Rule for the thor preprocessor
# Make sure we always run the preprocessor in the source directory
.chdl.cc:
	cd $(VPATH); pepp $<

# Rule for building a library
$(LIB):	$(OBJS)
	rm -f $(LIB)
	ar cq $(LIB) $(OBJS)
	ranlib $(LIB)

# Rule for installing a library
$(LIBDIR)/$(LIB):	$(LIB)
		rm -f $(LIBDIR)/$(LIB)
		ln $(LIB) $(LIBDIR)

# Rule for making a star list for inclusion by make
$(STAR_MK).mk:	make.template
		rm -f $(STAR_MK).mk
		genStarList $(DOMAIN) $(DOMAIN_2) > $(STAR_MK).mk

# Rule for making a star list module for pulling stars out of libraries
$(STAR_MK).c:	make.template
		rm -f $(STAR_MK).c
		cd $(VPATH); genStarTable $(DOMAIN) $(DOMAIN_2) > $(STAR_MK).c

$(STAR_MK).o:	$(STAR_MK).c

# Standard "all" for stars directories
stars_all:	$(SRCS) $(LIB) $(STAR_MK).o

# Rule for installing both the star library and star list
stars_install:	all $(LIBDIR)/$(LIB) $(LIBDIR)/$(STAR_MK).o
		@echo Installation complete.

# Rule for installing the star list

$(LIBDIR)/$(STAR_MK).o:	$(STAR_MK).o
		rm -f $(LIBDIR)/$(STAR_MK).o
		ln $(STAR_MK).o $(LIBDIR)

# "make sources" will do SCCS get on anything where SCCS file is newer.
sources:	$(EXTRA_SRCS) $(SRCS) $(HDRS) make.template

CRUD=*.o core *~ *.bak ,* LOG*
clean:
	rm -f $(CRUD)

# Make things "even cleaner".  Removes libraries, generated .cc and .h
# files from preprocessor, etc.
realclean:
	rm -f $(CRUD) $(LIB) $(PL_SRCS:.pl=.h) $(PL_SRCS:.pl=.cc) $(REALCLEAN_STUFF)

DEPEND_INCL=$(INCL) $(C_INCL)

depend:		$(SRCS) $(HDRS)
		cd $(VPATH); \
                $(DEPEND) $(DEPEND_INCL) $(SRCS)|\
                        cat make.template - > makefile.n;\
                mv -f makefile.n makefile

makefile:	make.template $(MDEPS)
		cd $(VPATH); rm -f makefile; cp make.template makefile; \
		make -f make.template sources depend
		if [ ! -f makefile ]; then ln -s $(VPATH)/makefile makefile; fi
		@echo makefile remade -- you must rerun make.
		exit 1

makefiles:	makefile

TAGS:		$(SRCS)
		etags $(SRCS)

# Rule for detecting junk files

checkjunk:
	@checkextra -v $(SRCS) $(HDRS) $(EXTRA_SRCS) $(OBJS) $(LIB) \
		$(STAR_MK).o $(EXTRA_DESTS) makefile make.template SCCS

# "check" does not print anything if nothing is being edited.
sccsinfo:
	@sccs check || true

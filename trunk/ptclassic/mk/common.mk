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
	cd $(VPATH); ptlang $< ; mv *.t ../doc/stars

.pl.h:
	cd $(VPATH); ptlang $< ; mv *.t ../doc/stars

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

$(STAR_MK).mk:	make.template
		rm -f $(STAR_MK).mk
		genStarList $(DOMAIN) $(DOMAIN_2) > $(STAR_MK).mk

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

makefile:	make.template
		cd $(VPATH); rm -f makefile; cp make.template makefile; \
		make -f make.template sources depend
		if [ ! -f makefile ]; then ln -s $(VPATH)/makefile makefile; fi
		@echo makefile remade -- you must rerun make.
		exit 1

TAGS:		$(SRCS)
		etags $(SRCS)

# Rule for detecting junk files

checkjunk:
	@checkextra -v $(SRCS) $(HDRS) $(EXTRA_SRCS) makefile make.template SCCS

sccsinfo:
	@if sccs check; then true; else sccs info; fi

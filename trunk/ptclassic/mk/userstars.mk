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
# Date of creation: 10/21/94
# Written by: E. A. Lee
#
# This file is designed to be used by a user with custom stars
# that are to be dynamically linked. It is only required when
# the compilation options built into the dynamic linker are
# not adequate.  For example, not all include files are in the
# include paths that are specified by default.  The user should
# create a makefile in the same directory where the stars are
# defined.  This should be called "make.template".  I minimal
# version of this is as shown below (the stars are assumed to
# be in the DE domain):
#
#	make.template:
#	---------------------------------------------------------
#	| ROOT = $(PTOLEMY)					|
#	| VPATH = .						|
#	| include $(ROOT)/mk/config-$(PTARCH).mk			|
#	| INCL = -I$(ROOT)/src/domains/de/kernel -I$(KERNDIR)	|
#	| PL_SRCS = DEMyFirstStar.pl DEMySecondStar.pl		|
#	| DOMAIN = DE						|
#	| include $(ROOT)/mk/userstars.mk			|
#	---------------------------------------------------------
#	
# The above file assumes the PTOLEMY environment variable is set to point
# to the home directory of the Ptolemy installation (e.g. /users/ptolemy).
#
# It also assumes the PTARCH environment variable is set to specify the target
# architecture (such as "sun4").
#
# The VPATH variable specifies where the sources are.  If the directory name
# in which your stars are stored includes the string "/src/", then the
# object files will be put in a corresponding directory with "src" replaced
# by $(PTARCH).  This allows your stars to have several object files for
# different architectures simultaneously.
#
# The include statement includes a standard Ptolemy makefile for the
# specified architecture.  You may want to look at that makefile. 
#
# The INCL definition specifies one or more directories to be searched
# for include files.  You need at least the kernel directory for the domain
# you are using.  If your stars are derived from standard stars, you will
# also need the directory in which those standard stars are defined.
# Also, very few Ptolemy stars will get by without the ptolemy kernel
# directory, specified by -I$(KERNDIR).
#
# The DOMAIN variable gives the domain name.
#
# To automatically generate all dependencies, the above makefile should
# first be created in a file called "make.template".  The following
# operations will then create the makefile:
#	make -f make.template depend
#
# At this point, if you type "make all", your custom DE stars will be compiled.
# They can then be dynamically linked into Ptolemy.
#
# Note that if this were all you wanted to do, then it would have been
# unnecessary to create the makefile.  The dynamic linker would take care
# of things for you.  But having the above makefile, you can now customize
# it to perform custom functions.

# The .cc files
SRCS=$(PL_SRCS:.pl=.cc)

# object files for all stars
OBJS= $(SRCS:.cc=.o)

# header files for all stars
HDRS= $(SRCS:.cc=.h)

EXTRA_SRCS=$(PL_SRCS)

all:	makefile $(OBJS)

.SUFFIXES:	.o .cc .h .pl

# Rule for compiling C++ files
.cc.o:
	$(CPLUSPLUS) $(USER_CC_SHAREDFLAGS) $(GPPFLAGS) -I$(VPATH) $(INCL) -c $<

# Rule for compiling with cc
.c.o:
	$(CC) $(USER_C_SHAREDFLAGS) $(CFLAGS) $(C_INCL) -c $<

# Note that forcing the installation of ptlang might not be the best
# thing to do, it would be best if 'make sources' did not touch the
# bin.$(PTARCH) directory, so we check to see if there is a ptlang in the
# obj.$(PTARCH)/ptlang directory and use it.  This is awkward, but the
# GNU tools do something similar

# ptlang binary in the obj directory
PTLANG_IN_OBJ=$(PTOLEMY)/obj.$(PTARCH)/ptlang/ptlang

# Use either the ptlang binary in the obj directory or just use ptlang
PTLANG= `if [ -f $(PTLANG_IN_OBJ) ]; \
	then echo $(PTLANG_IN_OBJ) ; \
	else echo ptlang; fi`

# Rules for running the ptlang processor
# Make sure we always run the preprocessor in the source directory
.pl.cc:
	cd $(VPATH); $(PTLANG) $< 

.pl.h:
	cd $(VPATH); $(PTLANG) $< 

# "make sources" will do SCCS get on anything where SCCS file is newer.
sources:	$(EXTRA_SRCS) $(SRCS) $(HDRS) make.template 
CRUD=*.o core *~ *.bak ,* LOG* $(KRUFT) 
clean:
	rm -f $(CRUD)

# Make things "even cleaner".  Removes libraries, generated .cc and .h
# files from preprocessor, etc.
realclean:
	rm -f $(CRUD) $(PL_SRCS:.pl=.h) $(PL_SRCS:.pl=.cc) $(PL_SRCS:.pl=.t) \
	$(REALCLEAN_STUFF)

DEPEND_INCL=$(INCL) $(C_INCL)

depend:		$(SRCS) $(HDRS)
		cd $(VPATH); \
                $(DEPEND) $(DEPEND_INCL) $(SRCS)|\
                        cat make.template - > makefile.n;\
                mv -f makefile.n makefile

makefile:	make.template $(MDEPS)
		cd $(VPATH); rm -f makefile; cp make.template makefile; \
		$(MAKE) -f make.template sources depend
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

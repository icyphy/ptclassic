# This is a Ptolemy version of xgraph, where the only difference is that
# it accepts binary input if you use the -binary command line switch.
#
# See: http://ptolemy.eecs.berkeley.edu/~cxh/sapub/xgraph.html
#
# Author: Christopher Hylands
# Version identification:
# @(#)makefile.in	1.8 01/28/99
#
# Copyright (c) 1990-1999 The Regents of the University of California.
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

# This makefile.in is based on the Tk8.0 makefile.in, so:
# This software is copyrighted by the Regents of the University of
# California, Sun Microsystems, Inc., and other parties.  The following
# terms apply to all files associated with the software unless explicitly
# disclaimed in individual files.
# 
# The authors hereby grant permission to use, copy, modify, distribute,
# and license this software and its documentation for any purpose, provided
# that existing copyright notices are retained in all copies and that this
# notice is included verbatim in any distributions. No written agreement,
# license, or royalty fee is required for any of the authorized uses.
# Modifications to this software may be copyrighted by their authors
# and need not follow the licensing terms described here, provided that
# the new terms are clearly indicated on the first page of each file where
# they apply.
# 
# IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY
# FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
# ARISING OUT OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY
# DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE
# IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE
# NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
# MODIFICATIONS.
# 
# GOVERNMENT USE: If you are acquiring this software on behalf of the
# U.S. government, the Government shall have only "Restricted Rights"
# in the software and related documentation as defined in the Federal 
# Acquisition Regulations (FARs) in Clause 52.227.19 (c) (2).  If you
# are acquiring the software on behalf of the Department of Defense, the
# software shall be classified as "Commercial Computer Software" and the
# Government shall have only "Restricted Rights" as defined in Clause
# 252.227-7013 (c) (1) of DFARs.  Notwithstanding the foregoing, the
# authors grant the U.S. Government and others acting in its behalf
# permission to use and distribute the software in accordance with the
# terms specified in this license.
 
# Default top-level directories in which to install architecture-
# specific files (exec_prefix) and machine-independent files such
# as scripts (prefix).  The values specified here may be overridden
# at configure-time with the --exec-prefix and --prefix options
# to the "configure" script.

prefix =		/usr/local
exec_prefix =		/usr/local

# Directory from which the program wish should be referenced by scripts:
BIN_DIR =		$(exec_prefix)/bin

# The following definition can be set to non-null for special systems
# like AFS with replication.  It allows the pathnames used for installation
# to be different than those used for actually reference files at
# run-time.  INSTALL_ROOT is prepended to $prefix and $exec_prefix
# when installing files.
INSTALL_ROOT =

# Top-level directory for manual entries:
MAN_INSTALL_DIR =	$(INSTALL_ROOT)$(prefix)/man

# Directory in which to install manual entry for pxgraph:
MAN1_INSTALL_DIR =	$(MAN_INSTALL_DIR)/man1

# A "-I" switch that can be used when compiling to make all of the
# X11 include files accessible (the configure script will try to
# set this value, and will cause it to be an empty string if the
# include files are accessible via /usr/include).
X11_INCLUDES =		-I/usr/openwin/include

# Linker switch(es) to use to link with the X11 library archive (the
# configure script will try to set this value automatically, but you
# can override it).
X11_LIB_SWITCHES =	-lX11

# Libraries to use when linking.  This definition is determined by the
# configure script.
LIBS =  $(X11_LIB_SWITCHES) -lm #-lc

# To change the compiler switches, for example to change from -O
# to -g, change the following line:
CFLAGS = -O

# Some versions of make, like SGI's, use the following variable to
# determine which shell to use for executing commands:
SHELL =		/bin/sh

# Tk used to let the configure script choose which program to use
# for installing, but there are just too many different versions of
# "install" around;  better to use the install-sh script that comes
# with the distribution, which is slower but guaranteed to work.

INSTALL = ../../../src/pxgraph/pxgraph/install-sh -c
INSTALL_PROGRAM =	${INSTALL}
INSTALL_DATA =		${INSTALL} -m 644

#----------------------------------------------------------------
# The information below is modified by the configure script when
# Makefile is generated from Makefile.in.  You shouldn't normally
# modify any of this stuff by hand.
#----------------------------------------------------------------

AC_FLAGS =		 -DNEED_STRICMP=1 
SRC_DIR =		../../../src/pxgraph/pxgraph

#----------------------------------------------------------------
# The information below should be usable as is.  The configure
# script won't modify it and you shouldn't need to modify it
# either.
#----------------------------------------------------------------


CC =		cc
CC_SWITCHES =	${CFLAGS} -I${SRC_DIR} \
	${X11_INCLUDES} ${AC_FLAGS} ${PROTO_FLAGS}

OBJS = xgraph.o xgX.o hard_devices.o dialog.o hpgl.o mif.o ps.o idraw.o xtb.o \
	st.o params.o

SRCS = $(SRC_DIR)/xgraph.c $(SRC_DIR)/xgX.c $(SRC_DIR)/hard_devices.c \
	$(SRC_DIR)/dialog.c $(SRC_DIR)/hpgl.c $(SRC_DIR)/mif.c \
	$(SRC_DIR)/ps.c $(SRC_DIR)/idraw.c $(SRC_DIR)/xtb.c \
	$(SRC_DIR)/st.c $(SRC_DIR)/params.c

HDRS = $(SRC_DIR)/dialog.h $(SRC_DIR)/hard_devices.h \
	$(SRC_DIR)/params.h $(SRC_DIR)/plotter.h $(SRC_DIR)/st.h \
	$(SRC_DIR)/xgout.h $(SRC_DIR)/xgraph.h $(SRC_DIR)/xtb.h

.c.o:
	$(CC) -c $(CC_SWITCHES) $<

all:	makefile pxgraph

pxgraph:	$(OBJS)
	rm -f pxgraph
	$(CC)  -o $@ $(OBJS) $(LIBS)

config.status: $(SRC_DIR)/makefile.in $(SRC_DIR)/configure
	$(SRC_DIR)/configure

makefile: config.status
	$(SHELL) config.status	

install:	makefile
	$(INSTALL_PROGRAM) pxgraph $(BIN_DIR)/pxgraph
	$(INSTALL_DATA) $(SRC_DIR)/pxgraph.man $(MAN1_INSTALL_DIR)/pxgraph.1

# "make clean" removes .o files and other cruft
clean:
		rm -f *.o core *~ \#* TAGS a.out pxgraph *#

distclean: clean
	rm -f makefile config.status config.cache config.log

# Exclude file for pxgraph tar file
PXGRAPH_EX = 	/tmp/pxgraph.ex
# GNU tar
GTAR = 		gtar
PXGRAPH_VERSION=11.3.4

# Create a tar file
DIST = /tmp/pxgraph$(PXGRAPH_VERSION).tar.gz
dist: $(DIST)
$(DIST): distclean $(PXGRAPH_EX)
	(cd ..; $(GTAR) -zchf $@ -X $(PXGRAPH_EX) pxgraph)
	rm -f $(PXGRAPH_EX)
$(PXGRAPH_EX):
	echo "SCCS\nmakefile\nconfig.log\nconfig.status\nconfig.cache" > $@ 


OTHER_SRCS =	ORIGINAL README.ANNOUNCE README.GENERAL README.HARDCOPY \
		README.INSTALL TAGS pxgraph.man README README.html \
		install-sh pxgraph.htm $(OBJS

# "make sources" will do SCCS get on anything where SCCS file is newer.
# Don't add pxgraph.htm, since rman might not be built yet
sources:	$(SRCS) $(HDRS) #pxgraph.htm

# Convert to html using rman
pxgraph.htm: pxgraph.man
	nroff -man $^ | rman -f HTML > $@


# Rule for detecting junk files
checkjunk:
	@checkextra -v $(SRCS) $(HDRS) $(OBJS) \
		$(OTHER_SRCS) \
		makefile \
		bitmaps examples SCCS

sccsinfo:
		sccs info

xgraph.o: $(SRC_DIR)/xgraph.c $(SRC_DIR)/xgout.h $(SRC_DIR)/xgraph.h \
	$(SRC_DIR)/xtb.h $(SRC_DIR)/hard_devices.h $(SRC_DIR)/params.h \
	$(SRC_DIR)/dialog.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/xgraph.c

xgX.o: $(SRC_DIR)/xgX.c $(SRC_DIR)/xgout.h $(SRC_DIR)/xgraph.h \
	$(SRC_DIR)/params.h $(SRC_DIR)/bitmaps/dot.11 \
	$(SRC_DIR)/bitmaps/mark1.11 $(SRC_DIR)/bitmaps/mark2.11 \
	$(SRC_DIR)/bitmaps/mark3.11 $(SRC_DIR)/bitmaps/mark4.11 \
	$(SRC_DIR)/bitmaps/mark5.11 $(SRC_DIR)/bitmaps/mark6.11 \
	$(SRC_DIR)/bitmaps/mark7.11 $(SRC_DIR)/bitmaps/mark8.11
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/xgX.c

hard_devices.o: $(SRC_DIR)/hard_devices.c $(SRC_DIR)/xgout.h \
	$(SRC_DIR)/xgraph.h $(SRC_DIR)/hard_devices.h $(SRC_DIR)/params.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/hard_devices.c

dialog.o: $(SRC_DIR)/dialog.c $(SRC_DIR)/xgout.h $(SRC_DIR)/xgraph.h \
	$(SRC_DIR)/hard_devices.h $(SRC_DIR)/xtb.h $(SRC_DIR)/params.h \
	$(SRC_DIR)/dialog.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/dialog.c

hpgl.o: $(SRC_DIR)/hpgl.c $(SRC_DIR)/xgout.h $(SRC_DIR)/xgraph.h \
	$(SRC_DIR)/plotter.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/hpgl.c

mif.o: $(SRC_DIR)/mif.c $(SRC_DIR)/xgout.h $(SRC_DIR)/xgraph.h \
	$(SRC_DIR)/plotter.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/mif.c

ps.o: $(SRC_DIR)/ps.c $(SRC_DIR)/xgout.h $(SRC_DIR)/xgraph.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/ps.c

idraw.o: $(SRC_DIR)/idraw.c $(SRC_DIR)/hard_devices.h $(SRC_DIR)/xgout.h \
	$(SRC_DIR)/xgraph.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/idraw.c

xtb.o: $(SRC_DIR)/xtb.c $(SRC_DIR)/xgraph.h $(SRC_DIR)/xtb.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/xtb.c

st.o: $(SRC_DIR)/st.c $(SRC_DIR)/st.h $(SRC_DIR)/xgraph.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/st.c

params.o: $(SRC_DIR)/params.c $(SRC_DIR)/st.h $(SRC_DIR)/params.h \
	$(SRC_DIR)/xgraph.h
	$(CC) -c $(CC_SWITCHES) $(SRC_DIR)/params.c

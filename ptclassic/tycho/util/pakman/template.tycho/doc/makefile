# Makefile for the pakman template.tycho /doc directory
#
# @Author: Christopher Hylands
#
# @Version: $Id$
#
# @Copyright (c) 1998 The Regents of the University of California.
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
##########################################################################


# Root of Tycho directory
ROOT =		../../../..

# True source directory
VPATH =		$(ROOT)/utils/pakman/template.tycho/doc

# Get configuration info
CONFIG =	$(ROOT)/mk/tycho.mk
include $(CONFIG)

# Sub directories to run make in.
DIRS =	

TCL_SRCS =

ITCL_SRCS = 

EXTRA_SRCS = $(TCL_SRCS) $(ITCL_SRCS) _makefile _index.html

MISC_FILES =	$(DIRS) img

# make checkjunk will not report OPTIONAL_FILES as trash
OPTIONAL_FILES =	

# 'make realclean' should remove tclIndex
REALCLEAN_STUFF =

# Do make sources so that we ensure that the subdirectories are checked out
all install: sources $(MISC_SRCS) tclIndex

# We don't include common.mk since we don't use the compiler here
include $(ROOT)/mk/no-compile.mk

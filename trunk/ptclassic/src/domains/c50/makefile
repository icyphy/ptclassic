# Makefile for C50 domain support
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
#		       
# Programmer: A. Baensch
# Created: 14 April 1995
# Version: $Id$
#
# Important!  Do not run this makefile while in $PTOLEMY/src/domains/c50.
# Instead run in from $PTOLEMY/obj.$ARCH/domains/c50 (through the symbolic
# link) -- that way a separate directory containing, say, Solaris and HPUX
# objects can be maintained.

ME = c50
ROOT = ../../..
VPATH = $(ROOT)/src/domains/$(ME)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

# source files
SHELL_SCRIPTS = asmc50 loadc50

include $(ROOT)/mk/topdomain.mk

sources:	subsources starHTML.idx $(SHELL_SCRIPTS)
	chmod a+x $(SHELL_SCRIPTS)

# Subdirectories with star directories in them
SUBDOMAINDIRS =

DIRS = kernel stars dsp targets $(SUBDOMAINDIRS)

include $(ROOT)/mk/domain.mk


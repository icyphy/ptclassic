# Makefile for SDF domain support
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
#		       
# Programmer:  J. T. Buck
# @(#)makefile	1.30	09/08/99
#
# Important!  Do not run this makefile while in ~ptolemy/src/domains/sdf.
# Instead run in from ~ptolemy/obj.$PTARCH/domains/sdf (through the symbolic
# link) -- that way a separate directory containing, say, Solaris and HPUX
# objects can be maintained.

ME = sdf
ROOT = ../../..
VPATH = $(ROOT)/src/domains/$(ME)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

include $(ROOT)/mk/topdomain.mk

# Subdirectories with star directories in them
SUBDOMAINDIRS = dfm dsp image tcltk matrix matlab atm contrib $(WILDFORCE)

DIRS = kernel stars loopScheduler targets $(SUBDOMAINDIRS)

# We only build the VIS subdomain under the cfront compiler on Solaris
# 2.5 and higher.  We test for sol2 and cfront in the architecture name
# and the presence of the VSDKHOME environment variable
ifneq ("$(findstring sol2,$(PTARCH))","")
ifneq ("$(findstring cfront,$(PTARCH))","")
ifdef VSDKHOME
	DIRS += vis
	SUBDOMAINDIRS += vis
endif
endif
endif

include $(ROOT)/mk/domain.mk


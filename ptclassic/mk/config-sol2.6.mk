# Config file to build on sun4 processor (SparcStation) running
# Solaris2.6 Maintenance Release 1 with egcs-1.0x

# $Id$

# Copyright (c) 1997-%Q% The Regents of the University of California.
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

ARCHFLAGS =	-DPTSOL2_5 -DPTSOL2_6 -D_PTHREAD_1003_1c

include $(ROOT)/mk/config-sol2.mk

CPLUSPLUS_COMPAT =  -I$(ROOT)/src/compat/cfront

# We must pase -DPT_EGCS so that make depend works properly.  Otherwise
# we get messages like:
# ../../src/compat/cfront/std.h:65: warning: No include path in which
#        to find sysent.h 
CPLUSPLUS = g++ $(CPLUSPLUS_COMPAT) -DPT_EGCS

# system libraries (libraries from the environment)
# No need to include -lg++ under egcs
SYSLIBS=$(CSYSLIBS)

# Build gthreads
INCLUDE_GTHREADS =	no

# Include the PN domain.
INCLUDE_PN_DOMAIN =	no


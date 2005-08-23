# Config file to build on SPARC processor running Solaris 10
# with gcc-2.95.1
# @(#)config-sol10.mk	1.1 05/31/05

# Copyright (c) 2005 The Regents of the University of California.
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

# Author:	Christopher Brooks

# 
# To build with the Sun CC binary, I used:
# setenv PTOLEMY /home/users/cxh/ptolemy
# setenv PTARCH sol10
# set path = ($PTOLEMY/bin $PTOLEMY/bin.sol10 /opt/SUNWspro/bin /usr/sfw/bin /usr/ccs/bin /bin /usr/bin /usr/ucb .)

include $(ROOT)/mk/config-sol8.mk
# PTSOL8 is read in src/compat/ptolemy/compat.h
ARCHFLAGS =	-DPTSOL2_5 -DPTSOL2_6 -DPTSOL7 -DPTSOL8 -DPTSOL10 -D_PTHREAD_1003_1c
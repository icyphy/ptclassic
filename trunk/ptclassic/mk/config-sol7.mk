# Config file to build on SPARC processor running Solaris7 (aka Solaris 2.7)

# $Id$

# Copyright (c) 1999 The Regents of the University of California.
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

# Author:	Christopher Hylands

# Many thanks to Mike Peck of Berkeley Camera Engineering (www.bce.com)
# for providing access to a Solaris 7 machine for testing this port

# If you don't have a compiler, see
# http://sunfreeware.com
# 
# To build with the Sun CC binary, I used:
# setenv PTOLEMY /home/users/cxh/ptolemy
# setenv PTARCH sol7
# set path = ($PTOLEMY/bin $PTOLEMY/bin.sol7 /opt/SUNWspro/bin /usr/ccs/bin /bin /usr/bin /usr/ucb .)

# PTSOL7 is read in src/compat/ptolemy/compat.h
ARCHFLAGS =	-DPTSOL2_5 -DPTSOL2_6 -DPTSOL7 -D_PTHREAD_1003_1c

include $(ROOT)/mk/config-sol2.mk

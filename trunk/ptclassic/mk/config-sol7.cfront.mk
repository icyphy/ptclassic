# Config file to build on SPARC processor running Solaris7 (aka Solaris 2.7)
# with SunSoft cc and CC 4.2

# @(#)config-sol7.cfront.mk	1.2 02/06/99

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

include $(ROOT)/mk/config-sol2.cfront.mk

# PTSOL7 is read in src/compat/ptolemy/compat.h
ARCHFLAGS =	-DPTSOL2_5 -DPTSOL2_6 -DPTSOL7 -D_PTHREAD_1003_1c

# Building gthreads fails because of messages like:
# ../include/pthread.h:179: conflicting types for `pthread_mutex_t'
# /usr/include/sys/types.h:361: previous declaration of `pthread_mutex_t'
# So, we cannot have the PN Domain

# Build gthreads
INCLUDE_GTHREADS =	no

# Include the PN domain.
INCLUDE_PN_DOMAIN =	no

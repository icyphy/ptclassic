# Configuration makefile to make on an HP-PA machine (7xx or 8xx) and HPUX9.x
# using GNU gcc and g++
#
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
#		       
# Programmer:  J. T. Buck, Neal Becker, Christopher Hylands

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Get the g++ definitions for shared libraries; we override some below.
# Comment the next line out if you don't want shared libraries.
include $(ROOT)/mk/config-g++.shared.mk

# Use the hpux10 config file and modify as necessary
include $(ROOT)/mk/config-hppa.mk

# Note that hppa does support shl_load() style dynamic linking, see
# $(PTOLEMY)/src/kernel/Linker.sysdep.h for more information.
# You may need to get the latest HP linker patch for shl_load to work.
# As of 4/95 the linker patch was PHSS_5083* for hpux9.x

# Misc. flags for OS version, if you are under HPUX9.x:
MISCCFLAGS =	-DUSE_SHLLOAD

# Used to flush the cache on the hppa.  (source is in the kernel/ directory)
# If you are running under HPUX9.x, you may want to 
# comment out FLUSH_CACHE and LIB_FLUSH_CACHE
#FLUSH_CACHE =	flush_cache.o
FLUSH_CACHE =
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
#LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o
LIB_FLUSH_CACHE =

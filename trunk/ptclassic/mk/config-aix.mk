# Configuration makefile to make on an IBM RS/6000
# or PowerPC machine under AIX3.2.5 using GNU gcc and g++
#
# Copyright (c) 1990-1995 The Regents of the University of California.
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
# $Id$

#	Programmer: Xavier Warzee (Thomson CSF)
#
#
#
# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

# Get the exported symbols of the Ptolemy libraries, and rules
include $(ROOT)/mk/config-aix-exp.mk

EXP_OPTS=$(subst $(LIBDIR),-Xlinker -bE:$(LIBDIR),$(LIBFILES:.a=.exp))

#
# Programs to use
#
LINKER =  g++
RANLIB =  ranlib
# Use gcc everywhere including in octtools
CC =      gcc
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC =        gcc -fwritable-strings

#OPTIMIZER =   -O2
OPTIMIZER =
WARNINGS =     -Wall -Wcast-qual
GPPFLAGS =     -g -DUSG -mminimal-toc $(CC_STATIC) $(MEMLOG) $(WARNINGS) 
$(OPTIMIZER)
# If you are not using gcc, then you might have problems with the WARNINGS flag
CFLAGS =  -g -DPOSIX -DUSG -mminimal-toc $(CC_STATIC) $(MEMLOG) $(WARNINGS) 
$(OPTIMIZER)

#
# Variables for the linker
#

CC_STATIC = #  -static

# We ship statically linked binaries, but other sites might want
# to remove the -static below
LINKFLAGS =    -L$(LIBDIR) -Xlinker -x -static -liconv $(EXP_OPTS)
LINKFLAGS_D =  -L$(LIBDIR) -g -static -liconv $(EXP_OPTS)

# uncomment the following line if you have the SMT ptf installed
SMTLIB        = -Xlinker -bI:/usr/lpp/X11/bin/smt.exp

#
# Directories to use
#
X11_INCSPEC =  -I/usr/lpp/X11/include
# Some AIX X11 installations need to use the pthreads library
X11_LIBSPEC =  $(SMTLIB) -L/usr/lib -lIM -L/usr/lpp/X11/lib -lX11 # -lpthreads
X11EXT_LIBSPEC=-lXext

VEM_X11_LIBSPEC = \
$(SMTLIB) -L/usr/lib -lXaw -lXmu $(X11EXT_LIBSPEC) -lIM -lXt -lX11 #-lpthreads

# system libraries (libraries from the environment)
SYSLIBS= -lbsd -lm

# octtools/attache uses this
TERMLIB_LIBSPEC = -lbsd

# Used to compile xv.  Use -traditional to avoid varargs problems
XV_CC =        gcc -traditional

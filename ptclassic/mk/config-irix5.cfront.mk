# Configuration makefile to build on SGI Indigo running Irix5.3 with
# SGI's `Delta-C++' compiler (This compiler is not cfront, but we call 
# all non-g++ compilers 'cfront' for historical reasons)
#
# $Id$
# Copyright (c) 1994-%Q% The Regents of the University of California.
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
#	Programmers: Dan Ellis (MIT Media Lab), Christopher Hylands
#
# NOTES on installing Ptolemy 0.5.2 beta under SGI IRIX 5.3 using the 
# vendor's compilers ($PTARCH=irix5.cfront)
# 
# I believe this is working well.  To install, you need only grab the
# archives pt-0.5.2beta.src.tar.gz and pt-0.5.2beta.other.src.tar.gz .
# Then proceed as ususal
# 
# (a) You have to make sure that the default $PTARCH is irix5.cfront 
#     rather than the plain irix5.  I did this by putting a patch 
#     into ~/.cshrc, which is where PTARCH is set from ~ptolemy/bin/ptarch. 
#     Thus, lines 12-16 .cshrc become:
#
#	# The arch script figures out what type of machine we are on.
#	if (! $?PTARCH) setenv PTARCH ` $PTOLEMY/bin/ptarch`
#	
#	# We want to use cfront - append .cfront suffix if not already there
#	setenv PTARCH `echo $PTARCH | sed -e "s/\..*//"`.cfront
#
#     You have to have the test because .cshrc is run many times with PTARCH 
#     already set, and you end up with ~/bin.irix5.cfront.cfront.cfront ...
#
# (b) Gnu make - must be the first `make' in $path.  I tried putting an 
#     alias in my .cshrc, but the make build invokes make under sh, which 
#     won't know the alias.
#
# (c) MATLAB - the install will automatically find your matlab libraries 
#     if it is installed on your system.  If you don't have matlab, you're 
#     OK (I assume).  However, if you do have matlab, the shipped libmat.a 
#     is not compiled with the special GOT-usage reducing flags, and it 
#     messes up the link of pigi.  The work around is to convert it into 
#     a shared object (linked at runtime) which works fine.  This is what 
#     I did (as root):
# 
#     cd /usr/tmp		# build here to avoid file locking problems
#     cp /usr/local/matlab/extern/lib/sgi/libmat.a .
#     ld -elf -shared -o libmat.so -all libmat.a           # creates libmat.so
#     cp libmat.so /usr/local/matlab/extern/lib/sgi/
#     ln -s /usr/local/matlab/extern/lib/sgi/libmat.so /usr/lib
#	# create symlink from /usr/lib so rld can find it - dumb of me
#
#    Then the pigi link automatically refers to the libmat.so in preference to 
#    linking in the libmat.a, and you're happy.

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

#
# Programs to use
#

# Define the C++ compiler - always have compat/cfront includes as 1st choice
CPLUSPLUS = 	CC -I$(ROOT)/src/compat/cfront

# Plain C compiler
CC = cc
# C compiler flags.  Defined by the arch-config mk.
CFLAGS =

# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC = cc -D_BSD_SIGNALS
# BSD_SIGNALS needed *only* for src/octtools/Xpackages/iv/timer.c, 
# but put it here for yuks

# command to generate dependencies (cfront users can try CC -M)
##DEPEND=g++ -MM
DEPEND=CC -M

# IRIX5.x does not have a ranlib
RANLIB = 	true

# linker to use for pigi and interpreter.
LINKER=CC
# system libraries (libraries from the environment)
SYSLIBS=-lm

OPTIMIZER =
# -g
# debug info uses too much disk space
##WARNINGS =	-Wall -Wcast-qual
WARNINGS  =

# Use -D_BSD_SIGNALS for src/kernel/SimControl.cc
#  see /usr/include/sys/signals.h for more info.
# Use -D_BSD_TIME for src/kernel/Clock.cc, see /usr/include/sys/time.h
MISC_DEFINES =	-D_BSD_SIGNALS -D_BSD_TIME


# dpwe: -G 0 is to disable accessing data off 'the global pointer' 
# (according to man CC); this is known to result in global space 
# overflow in large programs;  (man CC also suggests that 
# unless -nonshared is also specified, it is ignored anyway.)
#GOTFLAG = -G 0
# -xgot is the SGI hack to avoid overflows in the GOT by allowing 
# 32 bit offsets, or something.
GOTFLAG = -G 0 -xgot

GPPFLAGS =	$(GOTFLAG) $(MEMLOG) $(WARNINGS) $(MISC_DEFINES) $(OPTIMIZER)

#     -cckr   The traditional K&R/Version7 C with SGI extensions
CFLAGS =	$(GOTFLAG) -cckr $(OPTIMIZER)

#
# Variables for the linker
#
SYSLIBS =	-lm -lmld

# -s strips out debugging information, otherwise we get a 30Mb pigiRpc
# Unfortunately, -s causes the dreaded "Fatal error in c++patch" under 5.3 
# .. omit for now.
# -x is also useful, it removed local symbols, see the ld man page
# Unfortunately, linking -x on SGI's DCC generates executables that 
# just dump core.  So we'll have to settle for huge links and then 
# run 'strip' on the executables by hand later.
##LINKFLAGS =	-L$(LIBDIR) $(GOTFLAG) -Xlinker -s
LINKFLAGS =	-L$(LIBDIR) $(GOTFLAG)
##LINKFLAGS_D =	-L$(LIBDIR) $(GOTFLAG)
LINKFLAGS_D =	-L$(LIBDIR) $(GOTFLAG) 

#
# Directories to use
#
##X11_INCSPEC = -I/usr/X11/include
##X11_LIBSPEC = -L/usr/X11/lib -lX11

# S56 directory is only used on sun4.
S56DIR =

#
# Variables for miscellaneous programs
#
# Used by xv
#XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	/usr/bin/X11/bsdinst
XV_CC =		cc -cckr -DSVR4 -DXLIB_ILLEGAL_ACCESS

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE

# Matlab architecture
MATARCH = sgi

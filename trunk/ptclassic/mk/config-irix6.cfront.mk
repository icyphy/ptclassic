# Configuration makefile to make on an SGI machine with CC -64 or CC
# using SGI CC (note that the term 'cfront' is historical, the SGI CC probably
# is not cfront based)
#
# @(#)config-irix6.cfront.mk	1.14 02/04/99
# Copyright (c) 1996-1999 The Regents of the University of California.
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
# Programmer:  Dominique Ragot <ragot@ssi3.thomson-csf.fr>

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# common.mk looks at this variable to decide how to build shared libraries
USE_SHARED_LIBS = yes

#
#ifeq ($(USE_SHARED_LIBS),yes) 
# Use Position Independent Code to build shared libraries
C_SHAREDFLAGS =         
CC_SHAREDFLAGS =        
# mk/userstars.mk uses these vars
USER_C_SHAREDFLAGS =    $(C_SHAREDFLAGS)
USER_CC_SHAREDFLAGS =   $(CC_SHAREDFLAGS)
LIBSUFFIX =             so



# Location of SGI C++ shared libraries
SHARED_COMPILERDIR = 
SHARED_COMPILERDIR_FLAG = 
#SHARED_COMPILERDIR_FLAG = -L$(SHARED_COMPILERDIR)

# Command to build C++ shared libraries
SHARED_LIBRARY_COMMAND = CC -32 -shared -set_version 'vanillaUCB\#sgi7.0' $(SHARED_COMPILERDIR_FLAG) -o

# Command to build C shared libraries
CSHARED_LIBRARY_COMMAND = cc -32 -shared -set_version 'vanillaUCB\#sgi7.0' $(SHARED_COMPILERDIR_FLAG) -o

# List of libraries to search, obviating the need to set LD_LIBRARY_PATH
# See the ld man page for more information.  These path names must
# be absolute pathnames, not relative pathnames.
SHARED_LIBRARY_PATH = $(PTOLEMY)/lib.$(PTARCH):$(PTOLEMY)/octtools/lib.$(PTARCH):$(X11_LIBDIR):$(SHARED_COMPILERDIR):$(PTOLEMY)/tcltk/tcl.$(PTARCH)/lib
SHARED_LIBRARY_R_LIST = -Wl,-R,$(SHARED_LIBRARY_PATH)

#endif

#
# Programs to use
#

# Define the C++ compiler - always have compat/cfront includes as 1st choice
CPLUSPLUS =     CC -32 -I$(ROOT)/src/compat/cfront

# IRIX6.x does not have a ranlib
RANLIB = 	true
CC =		cc -32

# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC =	cc -32

# In config-$PTARCH.mk, we set the following variables.  We need to 
# use only the following variables so that we can use them elsewhere, say
# for non-optimized compiles.
# OPTIMIZER - The setting for the optimizer, usually -O2.
# MEMLOG    - Formerly used to log memory allocation and deallocation.
# WARNINGS  - Flags that print warnings.
# ARCHFLAGS - Architecture dependent flags, useful for determining which
#	      OS we are on.  Often of the form -DPTSOL2_4.
# LOCALCCFLAGS - Other architecture dependent flags that apply to all releases
#	      of the OS for this architecture for c++
# LOCALCFLAGS - Other architecture dependent flags that apply to all releases
#	      of the OS for this architecture for c
# USERFLAGS - Ptolemy makefiles should never set this, but the user can set it.

OPTIMIZER =	-g
# -Wsynth is a g++ flag first introduced in g++-2.6.x.
# Under gxx-2.7.0 -Wcast-qual will drown you with warnings from libg++ includes
#WARNINGS =	-Wall -Wsynth #-Wcast-qual 
WARNINGS =

ARCHFLAGS =	-DIRIX5 -DIRIX6	

# Use -D_BSD_SIGNALS for src/kernel/SimControl.cc
#  see /usr/include/sys/signals.h for more info.
# Use -D_BSD_TIME for src/kernel/Clock.cc, see /usr/include/sys/time.h
# dpwe: -G 0 is to disable accessing data off 'the global pointer' 
# (according to man CC); this is known to result in global space 
# overflow in large programs;  (man CC also suggests that 
# unless -nonshared is also specified, it is ignored anyway.)
## -xgot is the SGI hack to avoid overflows in the GOT by allowing 
# 32 bit offsets, or something.
GOTFLAG =       -G 0 -xgot
# WTC: Warning 3262 is 'unused variable'.  SGI compilers complain
# about the static variable file_id in every source file.
# Warning 3920: overriding virtual functions.
# You might want to turn these warnings off.
WOFF_FLAG = -woff 3262,3920
LOCALCCFLAGS =  $(GOTFLAG) $(WOFF_FLAG) -D_BSD_SIGNALS -D_BSD_TIME
LOCALCCFLAGS =  $(GOTFLAG) -D_BSD_SIGNALS -D_BSD_TIME
GPPFLAGS =      $(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
LOCALCFLAGS =   $(GOTFLAG) $(WOFF_FLAG) #-cckr
CFLAGS =        $(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)


# command to generate dependencies (cfront users can try CC -M)
##DEPEND=g++ -MM
DEPEND=CC -32 -M

#
# Variables for the linker
#

# linker to use for pigi and interpreter.
LINKER = CC -32 -quickstart_info
# C linker
CLINKER = cc -32
# system libraries for linking .o files from C files only
CSYSLIBS = 	-lm -lmld
# system libraries (libraries from the environment) for c++ files
SYSLIBS =	$(CSYSLIBS)

# -Xlinker -s strips out debugging information, otherwise we get a 30Mb
#  pigiRpc.  -s seems to cause core dumps inside collect, so we leave it out
# -x is also useful, it removed local symbols, see the ld man page
LINKFLAGS =	-L$(LIBDIR) $(GOTFLAG) # -Xlinker -s
LINKFLAGS_D =	-L$(LIBDIR) $(GOTFLAG)

#
# Directories to use
#
X11_INCSPEC = -I/usr/X11/include
X11_LIBSPEC = -L/usr/X11/lib -lX11

#
# Variables for miscellaneous programs
#
# Used by xv
#XV_RAND= RAND="-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	/usr/bin/X11/bsdinst
XV_CC =		$(CC) -cckr -DSVR4 -DXLIB_ILLEGAL_ACCESS

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE

# Matlab architecture
MATARCH = sgi

# Mathematica architecture
MATHEMATICAARCH = SGI

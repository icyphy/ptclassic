# Configuration makefile for an HP-PA machine (7xx or 8xx)
# running HP-UX 10.x with HP's C++ compler
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
# Programmers:   Sunil Samel (IMEC), Christopher Hylands
#
#
include $(ROOT)/mk/config-default.mk

# HPPA CC notes
# 1) If you purchase just the C++ compiler, you do _not_ get a C
#    compiler automatically.  You have to purchase the C compiler
#    separately.  You might be able to compile Ptolemy by using
#    HP CC and gcc, there are a few lines below that would need to be 
#    changed, see the comments.
# 2) To see what version of CC you are running, type 'what /usr/bin/CC'
# 3) You should probably install the latest and greatest CC and ld
#    patches off of the hp patch site.  As of 2/96, the site was
#    http://us-support.external.hp.com/
#    In a vanilla HPUX-10.20 installation, we installed
#	PHSS_9855 - s700_800 10.X HP C++ (A.10.24) with a correct eh/lib++.a
#	PHSS_9400 - s700_800 10.20 ld(1) cumulative patch
#	PHSS_9927 - s700_800 10.X ANSI C compiler cumulative patch
#	PHKL_8693 - s700 10.20 sys/time.h fix for select(2)/C++ defects
#


# Programs to use
#
RANLIB =	ranlib
# ptlang and islang use yacc, which is not necessarily part of hpux10.x
YACC =		bison -y
# islang uses lex, which is not necessarily part of hpux10.x
LEX =		flex
# src/kernel/makefile uses this to 
HP_AS =		/usr/ccs/bin/as

# C++ compiler to use.
# pigilib/POct.cc needs +a1 to initialize automatic aggregates.
# The following patch was released in early February, 1996:
# PHSS_6470      s700_800 10.X HP C++ patch version A.10.11
# If you don't have this patch, you may have problems compililng VHDLObjList.cc
# See above for patch information
CPLUSPLUS = 	CC -I$(ROOT)/src/compat/cfront +a1

# If you turn on debugging (-g) with cfront, expect ptcl and pigiRpc to be
# about 70Mb each.
GPPDEBUGFLAGS =
CDEBUGFLAGS =
# Used in octtools/vem/makefile
OCT_DEBUG_FLAGS =

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
#	      of the OS for this architecture for c++
# USERFLAGS - Ptolemy makefiles should never set this, but the user can set it.

# Flags for C++ compilation.  -DPOSTFIX_OP= is needed for cfront 2.1; it
# is not needed for 3.0.  Apparently -DPOSTFIX_OP= is needed for HPPA CC 3.20,
# but it is unneeded for HPPA CC 3.50.  You can use 'what /usr/bin/CC'
# to find out what version of HPPA CC you are using.

# If you are running under HPUX9, then remove -DPTHPUX10
ARCHFLAGS =	$(GPPDEBUGFLAGS) -DPTHPUX10 -D_REENTRANT -D_HPUX_SOURCE

# We need -D_CMA_NOWRAPPERS_ to build domains/pn/stars/PNSplice.cc, 
# pigilib/kernelCalls.cc, HOFBase.cc and others
# however, we can't have _CMA_NOWRAPPERS_ defined
# for octtools/Xpackages/rpc.c and some of the other files in pigilib
# So, we define it for C++ compiles, but not for C compiles.
LOCALCCFLAGS =	-D_CMA_NOWRAPPERS_
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)

# If you are using gcc and HPUX CC, then change the 'cc' to 'gcc'
# It turns out that when you order HPUX CC, you don't get a cc compiler,
# You must order it separately.
CC = 		cc
# Note that the bundled hppa cc compiler is non-ansi so it won't 
#  compile certain files in octtools that use ansi CPP, such as oct/io.h
# If you did not purchase the HPUX cc compiler, uncomment the line below
#OCT_CC =	gcc -fwritable-strings
# If you compile with gcc, then you will need to add -lgcc to the link
#  or you will get complaints about _eprintf missing
#LIBGCC_SPEC =	-L$(PTOLEMY)/gnu/hppa/lib/gcc-lib/hppa/2.7.2 -lgcc 

# -Aa turns on ansi c, needed for tkoct
# The cc man page says that defining _HPUX_SOURCE gives the same name space
# compatibility as -Ac
LOCALCFLAGS =	-DUSG $(CDEBUGFLAGS) -Aa
#LOCALCFLAGS = 	-DUSG $(CDEBUGFLAGS)

CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

# CC on HPs does not know the "-M" option as given in DEPEND.
# makedepend is part of X11
DEPEND =	makedepend
# common.mk look for USE_MAKEDEPEND
USE_MAKEDEPEND = 	yes

#
# Variables for the linker
#

# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = 	-Wl,-a,archive

# where the Gnu library and linker is
GNULIB =
# linker to use for pigi and interpreter
LINKER =	CC
# linker to use for programs that are not built with C++ files
CLINKER = $(CC)
# startup module
CRT0 =
# system libraries (libraries from the environment)
# /usr/lib/end.o is necessary for debugging with xdb under hpux9.x
# /opt/langtools/lib/end.o is necessary for debugging with xdb under hpux10.x
#SYSLIBS =	-lm /opt/langtools/lib/end.o
SYSLIBS =	$(LIBGCC_SPEC) -lm -L/opt/dce/lib/libcma.a -lcma /opt/langtools/lib/end.o

# system libraries for linking .o files from C files only
CSYSLIBS = $(SYSLIBS)

# ptlang uses this library to find alloca()
PTLANGLIB= -lPW

# If you don't strip symbols, then pigiRpc will be about 69Mb
# If you strip pigiRpc, then dynamic linking won't work
#      +A   Cause the linker ld(1) to use only archive libraries for all
#           occurrences of the -l option.  Also inform the C++ runtime
#           environment that no shared libraries will be used by the program.
# Note that the +A option seems to cause errors about:
# c++patch: could not find __head symbol. Did you link in libcxx.a?
# We use the +A flag to produce statically linked binaries so that we can
# more easily distribute the binaries.  If you don't need statically linked
# binaries, then don't use +A.  (The chatr command will tell you about
# whether a binary is dynamically or statically linked)

# link flags
LINKFLAGS =	-L$(LIBDIR) 
# link flags if debugging symbols are to be left
# In cfront, this will result in a 69Mb pigiRpc
#   -g causes chaos with /usr/lib/end.o and xdb, so we skip it
LINKFLAGS_D =	-L$(LIBDIR)

#
# Directories to use
#
X11_INCSPEC =	-I$(ROOT)/src/compat -I/usr/sww/X11R6/include
X11_LIBSPEC =	-L/usr/sww/X11R6/lib -lX11
#X11_INCSPEC =	-I$(ROOT)/src/compat
#X11_LIBSPEC =	-L/usr/lib/X11R5 -lX11

# Use -lSM -lICE for X11R6, don't use then for X11R5
X11EXT_LIBSPEC=	-L/usr/sww/X11R6/lib -lXext -lSM -lICE
#X11EXT_LIBSPEC=-lXext


#
# Variables for miscellaneous programs
#
# Used by xv
XV_RAND =	"-DNO_RANDOM -Drandom=rand"
XV_INSTALL =	bsdinst

# Used to flush the cache on the hppa.  (source is in the kernel/ directory)
FLUSH_CACHE =	flush_cache.o
# Destination of flush_cache.  Can't just subsitute $(LIBDIR)/flush_cache.o
# in pigiRpc/makefile, or the make will fail on other archs.
LIB_FLUSH_CACHE = $(LIBDIR)/flush_cache.o

# Defines to build xv
XMKMF =		/usr/sww/X11/bin/xmkmf
# -Ac turns on compatibility mode
# -DXLIB_ILLEGAL_ACCESS is needed to compile xv.c
XV_CC =		cc -Ac -DXLIB_ILLEGAL_ACCESS $(X11_INCSPEC) $(X11_LIBSPEC)

# Matlab architecture
MATARCH = hp700

# ptbin.mk uses this to decide whether to include the PN stars
# If you are under HPUX10, then the PN domain requires DCE threads.
#  you will need to install the DCE development set of the OS cds.
#  If you don't have a /usr/include/pthread.h, then you probably
#  don't have the DCE developement set installed.  If you don't have
#  this installed, set INCLUDE_PN_DOMAIN to no
#INCLUDE_PN_DOMAIN = no
INCLUDE_PN_DOMAIN = yes

# Ipus uses templates in a way that is compatible with g++, but not hppa.cfront
INCLUDE_IPUS_DOMAIN = no

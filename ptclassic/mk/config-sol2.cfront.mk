# Configuration makefile for Sun running Solaris2.3 and Sun CC3.0, CC4.0
# (Note that CC4.0 is not a cfront style compiler, it is native.)
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
# Programmers:  Richard Tobias, Christopher Hylands
#
# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------

include $(ROOT)/mk/config-default.mk

#
# Programs to use
#
RANLIB =	true
# C++ compiler to use.
CPLUSPLUS = 	CC -I$(ROOT)/src/compat/cfront

# common.mk looks at this variable to decide how to build shared libraries
USE_SHARED_LIBS = yes
#USE_SHARED_LIBS = no

# Using GNU make conditionals causes havoc while bootstrapping gcc,
# so we don't use them here, however, this is what the code would look like

#ifeq ($(USE_SHARED_LIBS),yes) 
# Use Position Independent Code to build shared libraries
# Octtools/Xpackages/rpc requires PIC instead of pic
C_SHAREDFLAGS =		-K PIC
# libcgstars.a requires PIC instead of pic
CC_SHAREDFLAGS =	-PIC
# mk/userstars.mk uses these vars
USER_C_SHAREDFLAGS =	$(C_SHAREDFLAGS)
USER_CC_SHAREDFLAGS =	$(CC_SHAREDFLAGS)
LIBSUFFIX =		so
SHARED_LIBRARY_COMMAND =	CC -G -o
CSHARED_LIBRARY_COMMAND =	cc -G -o
#endif

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

OPTIMIZER =
# If you turn on debugging (-g) with cfront, ptcl and pigiRpc could be
# about 70Mb each.  Also, with -g you will need at least 250Mb for the .o
# files, even before linking
#
# If you don't have the -g flag turned on then Sun CC 3.0.1 fails to compile 
# kernel/PortHole.cc with messages about: 
#	operands have incompatible types: op ":" 
GPPDEBUGFLAGS = -g
CDEBUGFLAGS =	-g

# -DPOSTFIX_OP= is needed for cfront 2.1; it is not needed for 3.0.

# Define PTSOL2_4 if you are on Solaris2_4
ARCHFLAGS =	-DPTSOL2_4

LOCALCCFLAGS =	$(GPPDEBUGFLAGS) -DSYSV -DSOL2 -D_REENTRANT
GPPFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCCFLAGS) $(USERFLAGS)
# If you are not using gcc, then you might have problems with the WARNINGS flag
LOCALCFLAGS =	$(CDEBUGFLAGS) -DSYSV -DSOL2 -D_REENTRANT
CFLAGS =	$(OPTIMIZER) $(MEMLOG) $(WARNINGS) \
			$(ARCHFLAGS) $(LOCALCFLAGS) $(USERFLAGS)

# CC does not recognize the "-M" option.
# "makedepend" is part of X11
# Note that makedepend does not produce dependencies as GNU g++ -M, so 
# you will probably want to run 'make depend' from an PTARCH that uses
# g++ -M instead of makedepend.  One way to do this would be to do:
# (Assuming you have g++ installed)
# 	cd $PTOLEMY; make PTARCH=sol2 depend
DEPEND =	makedepend
# common.mk looks for USE_MAKEDEPEND
USE_MAKEDEPEND =	yes
# Location of C++ includes like fstream.h.  Needed by makedepend
SYSTEM_DEPEND_INCL =	-I/opt/SUNWspro/SC3.0/include/CC
#
# Variables for the linker
#

# where the Gnu library and linker is
GNULIB=
# linker to use for pigi and interpreter
LINKER =	CC
# startup module
CRT0 =

# system libraries for linking .o files from C files only
CSYSLIBS =	-lsocket -lnsl -ldl -lm
# system libraries (libraries from the environment)
# Note that -lucb lib is needed to get bcopy and bzero for libMLelf.a
#  The current release of Mathematica uses these obsolete functions
#  instead of memcopy and memset.  Under g++, bcopy and bzero are defined
#  in libg++, so we don't need to include -lucb.
# Note that the Solaris ucb library is really broken, so it should be last
# in the list of libraries, or you may see really serious problems.
# If we don't have -lc before -lucb, then we get the wrong readdir
# and pigi fails to start up because of with the error message:
#  Error: no files matched glob pattern 
#	"/users/ptdesign/tcltk/itcl/lib/itk/init.*"
SYSLIBS =	$(CSYSLIBS) -lc -L/usr/ucblib -lucb

# If you don't strip symbols, then pigiRpc will be about 69Mb
# If you strip pigiRpc, then dynamic linking won't work
# The SunOS4.1.3 ld man page says:
# pigiRpc images produced with the -x ld flag will segv upon startup
# pigiRpc images produced with '-qoption ld -S' are still 69Mb

# Can't use -Bstatic here, or we won't be able to find -ldl, and
# dynamic linking will not work.
LINKFLAGS=-L$(LIBDIR) -R $(PTOLEMY)/lib.$(PTARCH):$(PTOLEMY)/octtools/lib.$(PTARCH):$(X11_LIBDIR):$(PTOLEMY)/tcltk/itcl.$(PTARCH)/lib/itcl:/usr/ucblib
# link flags if debugging symbols are to be left
LINKFLAGS_D=-L$(LIBDIR) -R $(PTOLEMY)/lib.$(PTARCH):$(PTOLEMY)/octtools/lib.$(PTARCH):$(X11_LIBDIR):$(PTOLEMY)/tcltk/itcl.$(PTARCH)/lib/itcl:/usr/ucblib	

#
# Directories to use
#
X11_INCSPEC =	-I/usr/openwin/include
X11_LIBDIR =	/usr/openwin/lib
X11_LIBSPEC =	-L$(X11_LIBDIR)  -lX11
X11EXT_LIBSPEC = -L$(X11_LIBDIR) -lXext

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR =

PURELINK =
PURIFY =	$(COLLECTOR) purify
QUANTIFY =

# Used by tcltk to build the X pixmap extension
XPM_DEFINES =	-DZPIPE -DSYSV $(X11_INCSPEC)

# Defines to build xv
XMKMF =		/usr/openwin/bin/xmkmf
# -Xs is needed for the varargs code in xv/tiff
# -w turns of warnings.  xv/bitmaps.h causes _lots_ of warnings
# -R$(X11LIB_DIR) is need so we can find the X libs at runtime,
#   otherwise, we will need to set LD_LIBRARY_PATH to include $(X11_LIBDIR) 
XV_CC =		cc -Xs -w $(X11_INCSPEC) \
		-DSVR4 -DSYSV -DDIRENT -DATT -DNO_BCOPY \
		$(X11_LIBSPEC) -R$(X11_LIBDIR)
XV_RAND = 	-DNO_RANDOM

# Matlab architecture
MATARCH = sol2

# Mathematica MathLink library
MATHLINKLIBNAME = MLelf

# Ipus uses templates in a way that is compatible with g++, but not sol2.cfront
INCLUDE_IPUS_DOMAIN = no

# Build gthreads
INCLUDE_GTHREADS =	yes

# Include the PN domain.
INCLUDE_PN_DOMAIN =	yes

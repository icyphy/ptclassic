#
# Config file to build on Linux
#
# $Id$

# Author: Alberto Vignani, FIAT Research Center, TORINO

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

include $(ROOT)/mk/config-g++.mk

#
# Programs to use
#
RANLIB = 	ranlib
CC =		gcc
# If we have g++, then compile Octtools with gcc.  ARCHs that are cfront
# based probably don't have gcc.
# OCT_CC is used in src/octtools/vem-{lib,bin}.mk
OCT_CC =	gcc


# /usr/include/stdlib.h may need to be modified for optimization to work.
# In particular, there could be a problem with __OPTIMIZE__ being defined
# and random().  Below is diff for the changes:
#    *** /usr/include/stdlib.h-dist	Mon Nov 29 08:36:11 1993
#    --- /usr/include/stdlib.h	Sun Aug  7 21:10:43 1994
#    ***************
#    *** 135,144 ****
#    --- 135,146 ----
#      extern void * setstate __P((void * __statebuf));
#      
#      #ifdef	__OPTIMIZE__
#    + #ifndef NO_RAND_OPTIMIZE
#      #define	random()		__random()
#      #define	srandom(seed)		__srandom(seed)
#      #define	initstate(s, b, n)	__initstate((s), (b), (n))
#      #define	setstate(state)		__setstate(state)
#    + #endif /* NO_RAND_OPTIMIZE */
#      #endif	/* Optimizing.  */
#      #endif	/* Use BSD.  */


# debug version
LINUXDEF =	-Dlinux #-D_GNU_SOURCE -D_BSD_SOURCE
OPTIMIZER =	-m486 -pipe -g

#
# 'production' version with optimization
#OPTIMIZER =	-O2 -m486 -fomit-frame-pointer -pipe
#LINUXDEF =	-Dlinux -DNO_RAND_OPTIMIZE #-D_GNU_SOURCE -D_BSD_SOURCE

# -Wsynth is new in g++-2.6.x, however 2.5.x does not support it
# Slackware is using 2.5.x, so we leave -Wsynth out for the time being.
WARNINGS =	-Wall -Wcast-qual -Wcast-align # -Wsynth
GPPFLAGS =	$(LINUXDEF) $(WARNINGS) $(OPTIMIZER) $(MEMLOG)
CFLAGS =	$(LINUXDEF) $(OPTIMIZER) -fwritable-strings

#
# Variables for the linker
#
# system libraries (libraries from the environment)
SYSLIBS=-lg++ -lm

LINKFLAGS=-L$(LIBDIR) -Xlinker -S -Xlinker -x -static
LINKFLAGS_D=-L$(LIBDIR) -g -static

# octtools/attache uses this
TERMLIB_LIBSPEC = -ltermcap

#
# Directories to use
#
# 'standard' (SLS-Debian) locations
#
#X11_INCSPEC = -I/usr/X386/include/X11
#X11INCL     = -I/usr/X386/include/X11
#X11_LIBSPEC = -L/usr/X386/lib/X11 -lX11
#
# X11R6
#
#X11_INCSPEC =	-I$(ROOT)/src/compat -I/usr/sww/X11R6/include
#X11_LIBSPEC =	-L/usr/sww/X11R6/lib -lX11
# Use -lSM -lICE for X11R6, don't use then for X11R5
#X11EXT_LIBSPEC=-lXext -lSM -lICE
#
# Slackware >=1.2 locations
#
X11_INCSPEC = -I/usr/X11/include
X11INCL     = -I/usr/X11/include
X11_LIBSPEC = -L/usr/X11/lib -lX11

# Flag that cc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = #-static

#S56DIR= $(ROOT)/vendors/s56dsp
S56DIR=

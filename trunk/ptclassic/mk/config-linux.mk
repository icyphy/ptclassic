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


LINUXDEF =	-Dlinux -D_GNU_SOURCE -D_BSD_SOURCE
#
# 'production' version
#OPTIMIZER =	-O2 -m486 -fomit-frame-pointer -pipe
#
# debug version
#
OPTIMIZER =	-m486 -pipe #-g
WARNINGS =	-Wall -Wcast-qual -Wcast-align
GPPFLAGS =	$(LINUXDEF) $(WARNINGS) $(OPTIMIZER) $(MEMLOG)
CFLAGS =	$(LINUXDEF) $(OPTIMIZER) -fwritable-strings

#
# Variables for the linker
#
# system libraries (libraries from the environment)
SYSLIBS=-lg++ -lbsd -lm

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
# Slackware >=1.2 locations
#
X11_INCSPEC = -I/usr/X11/include
X11INCL     = -I/usr/X11/include
X11_LIBSPEC = -L/usr/X11/lib -lX11

# Flag that cc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = -static

#S56DIR= $(ROOT)/vendors/s56dsp
S56DIR=

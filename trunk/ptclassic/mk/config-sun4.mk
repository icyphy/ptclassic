#
# Config file to build on sun4 processor (SparcStation) running SunOS4.1x
#
# $Id$

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

# Get the g++ definitions; we override some below.
include $(ROOT)/mk/config-g++.mk

#
# Programs to use
#
RANLIB =	ranlib
# Use gcc everywhere _except_ in octtools
CC =		gcc
OPTIMIZER =	-O2
WARNINGS =	-Wall -Wcast-qual -Wcast-align
GPPFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER)
CFLAGS =	-g $(OPTIMIZER)

#
# Variables for the linker
#

# Flag that cc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = 	-Bstatic

#
# Directories to use
#
X11_INCSPEC =	-I/usr/X11/include
X11_LIBSPEC =	-L/usr/X11/lib -lX11

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR = 	-collector=$(ROOT)/gnu/sun4/lib/gcc-lib/sun4/2.5.8/ld

PURELINK =	purelink $(COLLECTOR)
PURIFY =	purelink $(COLLECTOR) purify
QUANTIFY =	purelink $(COLLECTOR) quantify

# Variable for the Ariel DSP56000 board
S56DIR =	$(ROOT)/vendors/s56dsp

# Variables for local Matlab installation
MATLABDIR =	/usr/sww/matlab
MATLABLIBDIR =	-L$(MATLABDIR)/extern/lib/$(ARCH)

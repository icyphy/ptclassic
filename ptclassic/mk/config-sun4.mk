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
WARNINGS =	-Wall -Wcast-qual
GPPFLAGS =	-g $(MEMLOG) $(WARNINGS) $(OPTIMIZER)
CFLAGS =	-g $(OPTIMIZER)

#
# Variables for the linker
#
# Since we are compiling octtools with gcc, we don't want the 
# -lmm library linked in
OCTTOOLS_MM_LIB=

# Flag that gcc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that cc uses -Bstatic
CC_STATIC = 	-static

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
# -- If Matlab is installed, then MATLABDIR points to where MATLAB is installed
#    and MATLABLIBDIR points to the directory containing the Matlab libraries
# -- If Matlab is not installed, then MATLABDIR equals $ROOT/src/compat/matlab
#    and MATLABLIBIDR is undefined
MATLABDIR =	/usr/sww/matlab
MATLABLIBDIR =	-L$(MATLABDIR)/extern/lib/$(ARCH)

#
# Config file to build on sun4 processor (SparcStation) without optimization
#
# $Id$

# --------------------------------------------------------------------
# |  Please see the file ``config-default.mk'' in this directory!    |
# --------------------------------------------------------------------
include $(ROOT)/mk/config-default.mk

include $(ROOT)/mk/config-g++.mk

#
# Directories to use
#
X11_INCSPEC = -I/usr/X11/include
X11_LIBSPEC = -L/usr/X11/lib -lX11

RANLIB =	true

# Flag that cc expects to create statically linked binaries.
# Binaries that are shipped should be statically linked.
# Note that currently vem is built with cc, not gcc, so vem uses
# this flag. See also config-g++.mk
CC_STATIC = -Bstatic

# Variables for Pure Inc tools (purify, purelink, quantify)
COLLECTOR = -collector=$(ROOT)/gnu/sun4/lib/gcc-lib/sun4/2.5.8/ld
# cfront users will need a different value here
#COLLECTOR =

PURELINK = purelink $(COLLECTOR)
PURIFY = purelink $(COLLECTOR) purify
QUANTIFY = purelink $(COLLECTOR) quantify

S56DIR= $(ROOT)/vendors/s56dsp

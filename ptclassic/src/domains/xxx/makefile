# Makefile for XXX domain support
# Copyright (c) 1990 The Regents of the University of California.
#                       All Rights Reserved.
#		       
# Programmer:  E. A. Lee and D. Niehaus
# $Id$
#
# Important!  Do not run this makefile while in $PTOLEMY/src/domains/xxx.
# Instead run in from $PTOLEMY/obj.$PTARCH/domains/xxx (through the symbolic
# link) -- that way a separate directory containing, say, Solaris and HPUX
# objects can be maintained.

ME = xxx
ROOT = ../../..
VPATH = $(ROOT)/src/domains/$(ME)

# get configuration info
CONFIG=$(ROOT)/mk/config-$(PTARCH).mk
include $(CONFIG)

include $(ROOT)/mk/topdomain.mk

# Subdirectories with star directories in them
SUBDOMAINDIRS =

DIRS = kernel stars $(SUBDOMAINDIRS)

include $(ROOT)/mk/domain.mk


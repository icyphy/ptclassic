# Version identification:
# $Id$
# Copyright (c) 1995 The Regents of the University of California.
#                       All Rights Reserved.
# Date of creation: 2/27/95
# Written by: J. L. Pino

VPATH = .
CONFIG=$(ROOT)/mk/config-$(ARCH).mk
include $(CONFIG)
SDFDIR = $(ROOT)/src/domains/sdf/kernel
CGCDIR = $(ROOT)/lib/cgc
INCL = -I$(SDFDIR) -I$(KERNDIR) -I$(CGCDIR) $(WORM_INCL)
include $(ROOT)/mk/common.mk

.o.a:
ifeq ($(USE_SHARED_LIBS),yes)
	-rm -f $@
	$(SHARED_LIBRARY_COMMAND) $@ $^
else
	-rm -f $@
	$(AR) cq $@ $^ 
	$(RANLIB) $@
endif

all: $(WORM_ALL) 

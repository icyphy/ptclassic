# $Id$
#
# This file contains a list of the official CG stars.  It is intended
# to be included in other makefiles.  Before it is included, the variable
# CG must be defined to refer to the directory containing the object files
# for the stars.

CGSTARS= \
 $(CG)/CGRateChange.o \
 $(CG)/CGMultiIn.o \
 $(CG)/CGMultiOut.o \
 $(CG)/CGSource.o \
 $(CG)/CGThrough.o \
 $(CG)/CGSink.o \
 $(CG)/CGVarTime.o \
 $(CG)/CGSend.o \
 $(CG)/CGReceive.o \
 $(CG)/CGSpread.o \
 $(CG)/CGCollect.o 

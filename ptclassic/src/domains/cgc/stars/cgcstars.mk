# $Id$

# This file contains a list of the official CGC  stars.  It is intended
# to be included in other makefiles.  Before it is included, the variable
# CGC must be defined to refer to the directory containing the object files
# for the stars.

CGCSTARS = \
 $(CGC)/CGCDownSample.o \
 $(CGC)/CGCFIR.o \
 $(CGC)/CGCFloatDiff.o \
 $(CGC)/CGCFloatRamp.o \
 $(CGC)/CGCFork.o \
 $(CGC)/CGCIIDUniform.o \
 $(CGC)/CGCLMS.o \
 $(CGC)/CGCPrinter.o \
 $(CGC)/CGCSin.o \
 $(CGC)/CGCUpSample.o \
 $(CGC)/CGCXMgraph.o 


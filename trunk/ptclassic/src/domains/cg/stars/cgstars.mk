# $Id$
#
# This file contains a list of the official CG stars.  It is intended
# to be included in other makefiles.  Before it is included, the variable
# CG must be defined to refer to the directory containing the object files
# for the stars.

CGSTARS= \
 $(CG)/CGSource.o \
 $(CG)/CGThrough.o \
 $(CG)/CGSink.o

# $Id$
#
# This file contains a list of the official DDF stars.  It is intended
# to be included in other makefiles.  Before it is included, the variable
# DDF must be defined to refer to the directory containing the object files
# for the stars.

DDFSTARS= \
 $(DDF)/DDFSelf.o \
 $(DDF)/DDFCase.o \
 $(DDF)/DDFDownCounter.o \
 $(DDF)/DDFEndCase.o \
 $(DDF)/DDFLastOfN.o \
 $(DDF)/DDFRepeater.o \
 $(DDF)/DDFSelect.o \
 $(DDF)/DDFSwitch.o



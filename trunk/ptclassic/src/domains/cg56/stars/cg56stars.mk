# $Id$

# This file contains a list of the official CG56  stars.  It is intended
# to be included in other makefiles.  Before it is included, the variable
# CG56 must be defined to refer to the directory containing the object files
# for the stars.

CG56STARS = \
 $(CG56)/CG56Abs.o \
 $(CG56)/CG56Add.o \
 $(CG56)/CG56BiQuad.o \
 $(CG56)/CG56BlackHole.o \
 $(CG56)/CG56Cos.o \
 $(CG56)/CG56DC.o \
 $(CG56)/CG56DownSample.o \
 $(CG56)/CG56Fork.o \
 $(CG56)/CG56Gain.o \
 $(CG56)/CG56DummySource.o \
 $(CG56)/CG56DummySink.o \
 $(CG56)/CG56HostOut.o \
 $(CG56)/CG56IIDUniform.o \
 $(CG56)/CG56Integrator.o \
 $(CG56)/CG56Sub.o \
 $(CG56)/CG56Tone.o \
 $(CG56)/CG56UpSample.o \
 $(CG56)/CG56WriteFile.o


/*************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.

Stub file for mdefs.h provided for inclusion when Mathematica is
not installed on the local machine.

The commercial version of mdefs.h is a Mathematica source file,
Copyright 1986 through 1993 by Wolfram Research Inc.

The file defines several C language definitions for use with
Mathematica output as empty expressions for compatibility when
Mathematica is not installed on the local machine.

*************************************************************************/

#include <math.h>

#define Power(x, y)	
#define Sqrt(x)		

#define Abs(x)		

#define Exp(x)		
#define Log(x)		

#define Sin(x)		
#define Cos(x)
#define Tan(x)

#define ArcSin(x)
#define ArcCos(x)
#define ArcTan(x)

#define Sinh(x)
#define Cosh(x)
#define Tanh(x)

#define E		M_E
#define Pi		M_PI
#define Degree		(M_PI/180.0)

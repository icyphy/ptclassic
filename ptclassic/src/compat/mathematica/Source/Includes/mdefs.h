/*************************************************************************
Version identification:
@(#)mdefs.h	1.4	01/24/96

Copyright (c) 1986-1993 Wolfram Research, Inc. All Rights Reserved.

IN NO EVENT SHALL WOLFRAM RESEARCH INC. BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
IF WOLFRAM RESEARCH INC. HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

WOLFRAM RESEARCH INC. SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS
ON AN "AS IS" BASIS, AND WOLFRAM RESEARCH INC. HAS NO OBLIGATION TO
PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

This version of "mdefs.h" has been stripped of non-Unix declarations,
and is provided so that a MathLink interface to Mathematica will compile
if Mathematica is not installed on the local machine.  This stripped file
does not represent the actual implementation of the MathLink interface,
and should not be confused with the true version of "mdefs.h" that
comes with the Mathematica distribution.

This file is based on the MathLink source file "mdefs.h" (for
Mathematica 2.2) which defines the C language interface between
Mathematica and other programs through the MathLink protocol.

This stripped version is freely distributable by the permission of
Wolfram Research, Inc., 100 Trade Center Drive, Champaign, IL 61820-7237.

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

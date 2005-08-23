/*
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY
*/
// @(#)builtin.h	1.10 11/2/95
#ifndef _builtin_h
#define _builtin_h 1
#include <stream.h>

// abs functions
// with templates we will need only
// template<class T> inline T abs(T a) { return (a >= 0) ? a : -a;}

/* Can't use PTHPPA and PTAIX here, since we might not have included compat.h*/
#if defined(hpux) || defined(__hpux) || defined(_AIX) || defined(__sgi) || (defined(sparc) && (defined(__svr4__) || defined(__SVR4)) && !defined(__GNUC__))
#else
inline double abs(double a) { return (a >= 0) ? a : -a;}
inline int abs(int a) { return (a >= 0) ? a : -a;}
#endif
#endif

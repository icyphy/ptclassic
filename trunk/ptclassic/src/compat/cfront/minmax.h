/*
Copyright (c) 1990-%Q% The Regents of the University of California.
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
// $Id$
#ifndef _minmax_h
#define _minmax_h 1

// min and max functions, to match those defined in the libg++ minmax.h.

// with templates we will need only
// template<class T> inline T min(T a, T b) {return (a < b ? a : b);}
// template<class T> inline T max(T a, T b) {return (a > b ? a : b);}

inline double   min(double a, double b)     {return (a < b ? a : b);}
inline float    min(float a, float b)       {return (a < b ? a : b);}
inline int      min(int a, int b)           {return (a < b ? a : b);}
inline unsigned min(unsigned a, unsigned b) {return (a < b ? a : b);}
inline double   max(double a, double b)     {return (a > b ? a : b);}
inline float    max(float a, float b)       {return (a > b ? a : b);}
inline int      max(int a, int b)           {return (a > b ? a : b);}
inline unsigned max(unsigned a, unsigned b) {return (a > b ? a : b);}

#endif

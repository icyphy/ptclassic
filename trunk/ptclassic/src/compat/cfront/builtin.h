// $Id$
#ifndef _builtin_h
#define _builtin_h 1
#include <stream.h>

// abs functions
// with templates we will need only
// template<class T> inline T abs(T a) { return (a >= 0) ? a : -a;}

#if defined(hppa)
#else
inline double abs(double a) { return (a >= 0) ? a : -a;}
inline int abs(int a) { return (a >= 0) ? a : -a;}
#endif
#endif

// $Id$
#ifndef _builtin_h
#define _builtin_h 1
#include <stream.h>

// min and max functions, to match those defined in the libg++ builtin.h.

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

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 6/10/90

 This header file provides prototypes for miscellaneous
 library functions that are not members, together with some
 commonly used library functions.

*************************************************************************/

#ifndef _miscFuncs_h
#define _miscFuncs_h 1
#include <std.h>

extern "C" {
char*     strcat(char*, const char*);
char*     strchr(const char*, int);
int       strcmp(const char*, const char*);
char*     strcpy(char*, const char*);
int       strlen(const char*);
char*     strncat(char*, const char*, int);
int       strncmp(const char*, const char*, int);
char*     strncpy(char*, const char*, int);
char*     strrchr(const char*, int);
double	  pow(double,double);
}

#ifdef USG
inline char* index(const char* s, int c) { return strchr(s, c); }
inline char* rindex(const char* s, int c) { return strrchr(s, c); }
#else
extern "C" {
char*     index(const char*, int);
char*     rindex(const char*, int);
}
#endif

const char* expandPathName(const char*);

inline char* savestring (const char* txt) {
	return strcpy (new char[strlen(txt)+1], txt);
}
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

// I would prefer to make this int pow(int,int), but g++ gets confused
// because other func is extern "C".

int power(int base,int exp);
double power(double base,double exp) { return pow(base,exp);}

#endif

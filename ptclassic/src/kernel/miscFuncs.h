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

const char* hashstring(const char*);

// I would prefer to make this int pow(int,int), but g++ gets confused
// because other func is extern "C".

int power(int base,int exp);
inline double power(double base,double exp) { return pow(base,exp);}

#ifdef ADD_BUILTIN_INLINES
// these functions are needed to build with libg++ versions older
// than 1.39.0.

inline signed char min(signed char a, signed char b) { return (a < b)?a:b;}
inline unsigned char min(unsigned char a, unsigned char b) {return (a < b)?a:b;}

inline signed short min(signed short a, signed short b) {return (a < b) ?a:b;}
inline unsigned short min(unsigned short a, unsigned short b) {return (a < b)?a:b;}

inline signed int min(signed int a, signed int b) {return (a < b)?a:b;}
inline unsigned int min(unsigned int a, unsigned int b) {return (a < b)?a:b;}

inline signed long min(signed long a, signed long b) {return (a < b)?a:b;}
inline unsigned long min(unsigned long a, unsigned long b) {return (a < b)?a:b;}

inline float min(float a, float b) {return (a < b)?a:b;}

inline double min(double a, double b) {return (a < b)?a:b;}

inline signed char max(signed char a, signed char b) { return (a > b)?a:b;}
inline unsigned char max(unsigned char a, unsigned char b) {return (a > b)?a:b;}

inline signed short max(signed short a, signed short b) {return (a > b) ?a:b;}
inline unsigned short max(unsigned short a, unsigned short b) {return (a > b)?a:b;}

inline signed int max(signed int a, signed int b) {return (a > b)?a:b;}
inline unsigned int max(unsigned int a, unsigned int b) {return (a > b)?a:b;}

inline signed long max(signed long a, signed long b) {return (a > b)?a:b;}
inline unsigned long max(unsigned long a, unsigned long b) {return (a > b)?a:b;}

inline float max(float a, float b) {return (a > b)?a:b;}

inline double max(double a, double b) {return (a > b)?a:b;}
#endif

#endif

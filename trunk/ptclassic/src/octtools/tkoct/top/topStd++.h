#if !defined(_TOPSTDXX_H_)
#define _TOPSTDXX_H_ 1
/* 
    RCS: $Header$
    Author: Kennard White

    Copyright (c) 1990-93 The Regents of the University of California.
    All rights reserved.  See the file "./copyright.h" for full copyright
    notice, limitation of liability, and disclaimer of warranty provisions.
#include "copyright.h"				COPYRIGHTENDKEY
*/
/**
    topStd++.h :: top library

    90/10/01 KDW: Created as fixunix.h
    91/Mar/19 KDW: Copied from XPoleStd.h to TopStd.h
    91/Mar/29 KDW: Modified for C++ as TopStd++.h
**/

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <malloc.h>

#define constructor
#define destructor

extern "C" {
#include "topStd.h"
}
#endif /* _TOPSTDXX_H_ */

static const char file_id[] = "CG56Star.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 This is the baseclass for stars that generate assembly language code
 for the Motorola DSP 56000 family

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CG56Star.h"

// The following is defined in CG56Domain.cc -- this forces that module
// to be included if any CG56 stars are linked in.
extern const char CG56domainName[];

const char* CG56Star :: domain () const { return CG56domainName;}

// isa

ISA_FUNC(CG56Star, CGStar);

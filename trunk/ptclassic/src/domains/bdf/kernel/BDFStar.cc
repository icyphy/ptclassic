/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: J. T. Buck
 Date of creation: 1/8/91

*******************************************************************/

#include "BDFStar.h"
#include "BDFConnect.h"

/*******************************************************************

	class BDFStar methods

********************************************************************/

// BDF-specific initialize
void BDFStar :: prepareForScheduling() {}

// The following is defined in BDFDomain.cc -- this forces that module
// to be included if any BDF stars are linked in.
extern const char BDFdomainName[];

const char* BDFStar :: domain () const { return BDFdomainName;}


static const char file_id[] = "RanGen.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. Buck
 Date of creation: 1/9/92

This module provides the common seed for random-number generation.
It was formerly in Star.cc.

*******************************************************************/

#include <ACG.h>
#include "logNew.h"

// common random number generator for all Star classes.
// default seed is 1.

ACG* gen = 0;

// This class serves to initialize and clean up gen.  A pointer is
// used rather than a plain object so that the seed can be changed.

class RanGenOwner {
public:
	RanGenOwner() {
		LOG_NEW; gen = new ACG(1);
	}
	~RanGenOwner() {
		LOG_DEL; delete gen;
	}
};

static RanGenOwner buildIt;

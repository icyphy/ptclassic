
#include <std.h>
#include "StringState.h"

const int TOKSIZE = 80;

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  I. Kuroda
 Date of creation: 5/26/90
 Revisions:

 Functions for class StringState

**************************************************************************/


/*************************************************************************

	class StringState methods

**************************************************************************/


void StringState  :: initialize() {
	val = initValue;
}

// make known state entry
static StringState proto;
static KnownState entry(proto,"string");
	 


/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee and D. G. Messerschmitt
 Date of creation: 5/29/90
 Split off from Star.h by J. Buck

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SDFStar.h"
#include "SDFConnect.h"

/*******************************************************************

	class SDFStar methods

********************************************************************/


// SDF-specific initialize
void SDFStar :: prepareForScheduling() {
	repetitions = 0;
	noTimes = 0;
}

// firing SDF star
void SDFStar :: fire() {
	BlockPortIter next(*this);
	for(int i = numberPorts(); i > 0; i--)
		(next++)->grabData();
	go();
	next.reset();
	for(i = numberPorts(); i > 0; i--)
		(next++)->sendData();
}

// return execution time: define a default time of 5.
int SDFStar :: myExecTime() { return 5;}

// The following is defined in SDFDomain.cc -- this forces that module
// to be included if any SDF stars are linked in.
extern const char SDFdomainName[];

const char* SDFStar :: domain () const { return SDFdomainName;}


#ifndef _BDFStar_h
#define _BDFStar_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 1/8/91

 BDFStar is a Star that runs under the BDF Scheduler.

*******************************************************************/

#include "Star.h"
#include "Fraction.h"
#include "BDFConnect.h"
#include "Particle.h"

	////////////////////////////////////
	// class BDFStar
	////////////////////////////////////

class BDFStar : public Star  {

public:
	// my domain
	const char* domain() const;

	// domain specific initialization
	void prepareForScheduling();

};

#endif

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 10/2/90 (formerly part of DEStar.h)

 Definition of DERepeatStar (which fires itself repeatedly).

*******************************************************************/
#ifndef _DERepeatStar_h
#define _DERepeatStar_h 1
#include "DEStar.h"

	////////////////////////////////////
	// class DERepeatStar
	////////////////////////////////////

class DERepeatStar : public DEStar {

protected:
	// specify the feedback links to fire itself.
	InDEPort feedbackIn;
	OutDEPort feedbackOut;

	// access the feedback arc.
	void refireAtTime(float when);	// send next event.
	int canGetFired();		// can it be fired?

public:
	// define the common start function for this class
	// completionTime should be setup beforehand.
	void start();

	// constructor
	DERepeatStar();
};

#endif

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
#ifdef __GNUG__
#pragma interface
#endif

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
	void refireAtTime(double when);	// send next event.
	int canGetFired();		// can it be fired?

public:
	// initialization: generate the first event.
	// completionTime should be setup beforehand.
	void initialize();

	// constructor
	DERepeatStar();
};

#endif

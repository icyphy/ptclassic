/******************************************************************
Version identification:
@(#)DERepeatStar.h	1.12    05/28/98

Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

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
    friend class Resource;

protected:
	// specify the feedback links to fire itself.
	InDEPort* feedbackIn;
	OutDEPort* feedbackOut;

	// access the feedback arc.
	void refireAtTime(double when, double value = 0.0); // send next event.
	int canGetFired();		// can it be fired?

public:
	// initializaion: make the feedback connection
	/*virtual*/ void initialize();

	// initialization: generate the first event.
	// completionTime should be setup beforehand.
	void begin();

	// constructor
	DERepeatStar();

	// destructor
	~DERepeatStar();

        // class identification
        /* virtual */ int isA(const char*) const;

        // we need this function for an initial event to another time than 0.0
        void start(double);
};

#endif

#ifndef _DERCStar_h
#define _DERCStar_h 1
/**************************************************************************
Version identification:	$Id$

Author: Mudit Goel, Neil Smyth

Copyright (c) 1997-%Q% The Regents of the University of California.
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
*/
 
#ifdef __GNUG__
#pragma interface
#endif

#include <stdio.h>              // Needed for FILE
#include "Particle.h"
#include "DERepeatStar.h"
#include "Resource.h"
#include "DERCScheduler.h"
#include "DERCEventQ.h"
class Resource;

class DERCStar : public DERepeatStar
{
public:
	DERCStar();
	/* virtual */ virtual Block* makeNew() const;
	/* virtual */ virtual const char* className() const;
	/* virtual */ virtual int isA(const char*) const;
	/* virtual */ virtual double getDelay();
	/* virtual */ virtual SequentialList* getEvents ();
        /* virtual */ virtual void emitEvent(DERCEvent*, double ) {};
	Resource* resourcePointer;
	char resource[1024];
	double timeOfArrival;
	int priority;
	SequentialList* emittedEvents;
	FILE * Openoverflow ( char *name );
	void Printoverflow ( char *st );
	FILE * Openfiring ( char *name );
	void Printfiring ( char *st );
	void Closeflow ();
        int needsSharedResource;
        int schedPolicy;
	DERCEventQ* interruptQ;

protected:
	static FILE *fpoverflow;
	static FILE *fpfiring;
};
#endif

#ifndef _Nebula_h
#define _Nebula_h 1
#ifdef __GNUG__
#pragma interface
#endif


/*******************************************************************
 SCCS Version identification :
 $Id$

Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

 Programmers : Jose Luis Pino & T. M. Parks
 Date of Creation : 6/10/90
	
********************************************************************/

#include "GalIter.h"
#include "DynamicGalaxy.h"

class Star;

class Nebula {
    friend class NebulaIter;
public:
	// Constructor.
	Nebula(Star& self, Block* master);
	
	// Destructor.
	virtual ~Nebula();

	// Set the scheduler of the Nebula
	void setInnerSched(Scheduler* s) {sched = s;}

	// Generate the schedules of the nested Nebulas recursively.
	int generateSchedule();
 
        Scheduler* outerSched() { return selfStar.scheduler(); }
	Scheduler* innerSched() { return sched;} 

	void merge(Nebula*);
	void absorb(Nebula*);

	int run();

	virtual PortHole* clonePort(const PortHole*) const  = 0;
	
	virtual Nebula* newNebula(Block*) const = 0;

        Star& star() const { return selfStar; }
protected:
	// The Star part of the Nebula.
	Star& selfStar;

	// The star master if the Nebula is atomic.
	Block* master;

	DynamicGalaxy gal;
	Scheduler* sched;

};

// An iterator for NebulaList.
class NebulaIter : private GalStarIter {
public:
	NebulaIter(Nebula& n):GalStarIter(n.gal) {};
	Nebula* next() { return (Nebula*)GalStarIter::next();}
	Nebula* operator++(POSTFIX_OP) { return next();}
	GalStarIter::reset;
};

class NebulaPort {
public:
	NebulaPort(PortHole& self, const PortHole& p, const Nebula* parent);
	PortHole& real() { return pPort; }
	PortHole& asPort() { return selfPort;}
	/*virtual*/ int isItInput() {
		return pPort.isItInput();
	}
	/*virtual*/ int isItOutput() {
		return pPort.isItOutput();
	}
private:
	PortHole& selfPort;
	PortHole& pPort;
};

#endif


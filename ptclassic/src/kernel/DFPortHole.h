#ifndef _DFPortHole_h
#define _DFPortHole_h 1
#ifdef __GNUG__
#pragma interface
#endif
#include "PortHole.h"
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer:  J. Buck, E. A. Lee and D. G. Messerschmitt

DFPortHole is the base class for portholes in the various dataflow
domains.

******************************************************************/

        //////////////////////////////////////////
        // class DFPortHole
        //////////////////////////////////////////

// A DFPortHole is a porthole that moves a number of tokens in a
// somewhat predictable way.  If isDynamic() is FALSE, the number
// of tokens transferred is fixed and known at "compile time";
// otherwise, it varies according to a set of rules determined
// by the assocPort and assocRelation (not described here; see
// DynDFPortHole for more).

class DFPortHole : public PortHole
{
public:
	DFPortHole();

	// farSidePort is always DFPortHole or derived.  This
	// overrides PortHole::far.
	DFPortHole* far() const { return (DFPortHole*)farSidePort;}

	// Tell whether port uses old values
	int usesOldValues() const { return maxBackValue >= numberTokens;}

	// access the maximum delay
	int maxDelay() const { return maxBackValue;}

	// decCount, incCount, and setMaxArcCount are not virtual because
	// the virtual behavior is in the Geodesic.
	// Modify simulated counts on geodesics, for scheduling
	void decCount(int n);
	void incCount(int n);

	// set geodesic's maximum buffer size, where this can be determined
	// directly.
	void setMaxArcCount(int n);

	// class identification
	int isA(const char*) const;

	// associated control port (default: none)
	virtual PortHole* assocPort();

	// relation to associated port (default: none)
	virtual int assocRelation() const;

	// Function to alter only numTokens and delay.
	// We re-do porthole initialization if bufferSize changes
	virtual PortHole& setSDFParams(unsigned numTokens = 1,
				       unsigned maxPctValue=0);

	// is the port dynamic? (default: return 0)
	virtual int isDynamic() const;

	// The number of repetitions of the parent star, valid only
	// after the schedule is computed.
	int parentReps() const;
protected:
	int maxBackValue;	// maximum % argument allowed
	/* virtual */ int allocatePlasma(); // use local plasma
};

class MultiDFPort : public MultiPortHole {
public:
	MultiDFPort();
	~MultiDFPort();

	// Function to alter only numTokens and delay.
	// We re-do porthole initialization if bufferSize changes
	MultiPortHole& setSDFParams(unsigned numTokens = 1,
				    unsigned maxPstValue=0);

        // The setPort function is redefined to take one more optional
        // argument, the number of Particles produced
        MultiPortHole& setPort(const char* portName,
                          Block* parent,
                          DataType type = FLOAT,        // defaults to FLOAT
                          unsigned numTokens = 1);      // defaults to 1
protected:
        // The number of Particles consumed
        unsigned numberTokens;
};

#endif

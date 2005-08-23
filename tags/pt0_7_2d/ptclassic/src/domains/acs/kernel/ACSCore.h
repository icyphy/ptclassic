// This may look like C code, but it is really -*- C++ -*-
#ifndef _ACSCore_h
#define _ACSCore_h

/**********************************************************************
Copyright (c) 1998-1999 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA OR SANDERS, A LOCKHEED
MARTIN COMPANY, BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS
SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
OR SANDERS, A LOCKHEED MARTIN COMPANY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA AND SANDERS, A LOCKHEED MARTIN COMPANY
SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND
THE UNIVERSITY OF CALIFORNIA OR SANDERS, A LOCKHEED MARTIN COMPANY
HAVE NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.  COPYRIGHTENDKEY


 Programmers:  Eric Pauer (Sanders), Christopher Hylands
 Date of creation: 1/15/98
 Version: @(#)ACSCore.h	1.13 09/08/99

***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif


#include "ACSStar.h"
#include "ACSCorona.h"

class ACSCorona;

class ACSCore : public ACSStar {
  //JMS
  //friend ACSCGFPGATarget::getCorona();
public:

        // constructor takes a reference to a Corona
        // ACSCore(ACSCorona &, const char*);

	// constructor without Corona reference for use by
	// derrived core class default constructors.
	ACSCore(const char*);

//  ACSCore::~ACSCore(void);

	// virtual makeNew( ACSCorona & ) defined by each core
	// and used by Corona to build core list.
	virtual ACSCore* makeNew( ACSCorona & ) const { return (ACSCore *) NULL; }

	// get core category defined in the base core class
	// of each category.
	const char* getCategory() const { return category; }

        // my domain
        const char* domain() const;

        // class identification
        int isA(const char*) const;

	// virtual
	void go() {}

	// adds state to Corona states list
	// a rather odd compile problem without
	// the cast to ACSStar& ( addState actually
	// defined in Block.h ).
	inline void addState(State& s) {
		ACSCorona& corona = getCorona();
		((ACSStar&)corona).addState(s);
		ACSStar::addState(s);
	}

	// virtual identification for Fixed-Point cores used
	// by ACSTarget to determine if Plasma substitution is necessary.
	virtual int isFixedPoint() const { return FALSE; }

	// virtual identification for Code Generation cores used
	// by ACSTarget to determine if Code generation is called for instead
	// of simulation.
	virtual int isCG() const { return FALSE; }

	// Retrieve the PortHole with the given name
	// The first method returns a generic port, the second returns
	// a real PortHole with all aliases resolved
	GenericPort *genPortWithName (const char* name) { return ((Block&)getCorona()).genPortWithName(name); }
	const GenericPort *genPortWithName (const char* name) const { return ((Block&)getCorona()).genPortWithName(name); }
	PortHole *portWithName(const char* name) const { return  ((Block&)getCorona()).portWithName(name); }

	/* virtual */ State *stateWithName(const char* name);

  // JMS
	// dummy corona for virtual getCorona()
	static ACSCorona dummy;
	// returns reference to the core's corona
	virtual ACSCorona & getCorona() const { return dummy; }



protected:
	// category string determines core base class, implementation, etc..
        const char *category;

	// Corona interfaces core to galaxy, contains ports, parameters.
	// Corona is now defined in the derrived cores as a reference to
	// the derrived Corona class.
       	// ACSCorona & corona;
};
#endif //_ACSCore_h

// This may look like C code, but it is really -*- C++ -*-
#ifndef _ACSCore_h
#define _ACSCore_h

/**********************************************************************
Copyright (c) 1998 The Regents of the University of California.
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


 Programmers:  Eric Pauer (Sanders), Christopher Hylands
 Date of creation: 1/15/98
 Version: $Id$

***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif


#include "ACSStar.h"
#include "ACSCorona.h"

class ACSCorona;

class ACSCore : public ACSStar {
public:

        // constructor takes a reference to a Corona
        // ACSCore(ACSCorona &, const char*);

	// constructor without Corona reference for use by
	// derrived core class default constructors.
	ACSCore(const char*);

	// virtual makeNew( ACSCorona & ) defined by each core
	// and used by Corona to build core list.
	virtual ACSCore* makeNew( ACSCorona & ) const = 0;

	// get core category defined in the base core class
	// of each category.
	const char* getCategory() const { return category; }

        // my domain
        const char* domain() const;

        // class identification
        int isA(const char*) const;

	// pure virtual
	void go() = 0;

	// adds state to Corona states list
	// a rather odd compile problem without
	// the cast to ACSStar& ( addState actually
	// defined in Block.h ).
	inline void addState(State& s) {
		ACSCorona& corona = getCorona();
		((ACSStar&)corona).addState(s);
		ACSStar::addState(s);
	}

protected:

	// returns reference to the core's corona
	virtual ACSCorona & getCorona() const = 0;

	// category string determines core base class, implementation, etc..
	const char *category;

	// Corona interfaces core to galaxy, contains ports, parameters.
       	// ACSCorona & corona;

};
#endif //_ACSCore_h

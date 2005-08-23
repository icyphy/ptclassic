// This may look like C code, but it is really -*- C++ -*-
#ifndef _ACSCorona_h
#define _ACSCorona_h

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
 Version: @(#)ACSCorona.h	1.17 09/08/99

***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif


#include "ACSStar.h"
#include "DataStruct.h"
#include "ACSCore.h"
#include "ACSKnownCategory.h"

class ACSCore;

class ACSCorona : public ACSStar {	
public:

	// Default constructor initializes init core flag to zero ( no cores ).
	ACSCorona() : initCoreFlag(0) { }

	// This is a constructor with init core flag as argument.
	ACSCorona(int initCoreFlag_) : initCoreFlag(initCoreFlag_) { }
	// constructor needs to know where to look for core sources.
	virtual const char* getSrcDirectory() { return NULL; }

	// constructor needs a method to construct cores and add them
	// to the coreList.
	void addCores();

        // my domain
        const char* domain() const;

        // class identification
        int isA(const char*) const;

	// select ACSCore to be used by target
	int setCore(const char*);

	// register ACSCore in list
	static int registerCore(ACSCore &, const char*);

	// get available Core Categories
	StringList getCoreCategories() { 
		return ACSKnownCategory::getCategories();
	}

	// initialize cores name and parent members
	void initCores();

	/* virtual */ int isSDF() const { return TRUE; } // FIXME: DDF?

	// call the corresponding core method.
	/* virtual */ void initialize();
	/* virtual */ void wrapup();
	/* virtual */ void initCode();
	/* virtual */ int myExecTime();

	// add a splice star to the spliceClust list.  If atEnd
	// is true, append it to the end, otherwise prepend it.
	void addSpliceStar(ACSStar* s, int atEnd);

       // Generate declaration, initialization and function codes for
       // command-line arguments
       /* virtual */ StringList cmdargStates(Attribute a=ANY);
       /* virtual */ StringList cmdargStatesInits(Attribute a=ANY);
       /* virtual */ StringList setargStates(Attribute a=ANY);	
       /* virtual */ StringList setargStatesHelps(Attribute a=ANY);	

       // Generate declarations and initialization code for PortHoles
       /* virtual */ StringList declarePortHoles(Attribute a=ANY);
       /* virtual */ StringList initCodePortHoles(Attribute a=ANY);

       // Generate declarations and initialization code for States
       /* virtual */ StringList declareStates(Attribute a=ANY);
       /* virtual */ StringList initCodeStates(Attribute a=ANY);

	/* virtual */ StringList BufferIndex(const ACSPortHole * port, const char * name, const char * offset);

	/* virtual */ int isItFork(); // { return forkId; }

	/* virtual */ int setTarget(Target* t);

	/* virtual */ int runCG();

  // JMS
	// Core used in current run ( FIXME: could be a list also ).
	ACSCore *currentCore;

protected:

	// calls the core->go() method.
	/* virtual */ void go();

	// List of available cores
        SequentialList coreList;


	// Flag used to indicate if coreList needs to be constructed.
	int initCoreFlag;


};
#endif //_ACSCorona_h

#ifndef _ACSStar_h
#define _ACSStar_h

/*
@Version: @(#)ACSStar.h	1.13 09/08/99

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

    Author:	Eric Pauer (Sanders) and Christopher Hylands
    Created:	15 January 1998

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "ACSPortHole.h"
#include "Attribute.h"

// The following include is not really required for this file,
// but any star for this domain will need it, so we include it here.
#include "ACSPortHole.h"

// New attribute bit definition
extern const bitWord AB_VARPREC /* = 0x40 */;

extern const Attribute ANY /* = {0,0} */;

// New attribute for ports and states of type FIX/FIXARRAY.
// These attributes are ignored for ports/states of other types.

// fixed point precision may change at runtime;
// declare a precision variable holding the actual precision
extern const Attribute A_VARPREC  /* = {AB_VARPREC,0}*/;
// fixed point precision does not change at runtime (the default)
extern const Attribute A_CONSTPREC /* = {0,AB_VARPREC}*/;


class ACSStar : public CGStar
{
public:
    // Class identification.
    /*virtual*/ int isA(const char*) const;

    // Domain identification.
    /*virtual*/ const char* domain() const;

    // For simulation we just call the base-class simulation star.
    // for code-generation we call something else.
    //int run() { if ( ((ACSTarget*)(target()))->isCG() ) return(CGStar::run()); else return(DataFlowStar::run()); } // FIXME
    int run(); // { return(DataFlowStar::run()); } // FIXME
    virtual int runCG() { return FALSE; }

    // add a splice star to the spliceClust list.  If atEnd
    // is true, append it to the end, otherwise prepend it.
    virtual void addSpliceStar(ACSStar* s, int atEnd) { if ( s || atEnd ) return; }

    // unfortunate, but we need to make special treatment for
    // Spread/Collect stars when splicing. Note that Spread/Collect
    // are not regular stars
    // If Spread, redefine to return -1, if Collect, return 1, otherwise 0.
    virtual int amISpreadCollect() { return FALSE; }

    // Generate declaration, initialization and function codes for
    // command-line arguments
    virtual StringList cmdargStates(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }
    virtual StringList cmdargStatesInits(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }
    virtual StringList setargStates(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }	
    virtual StringList setargStatesHelps(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }	

    // Generate declarations and initialization code for PortHoles
    virtual StringList declarePortHoles(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }
    virtual StringList initCodePortHoles(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }

    // Generate declarations and initialization code for States
    virtual StringList declareStates(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }
    virtual StringList initCodeStates(Attribute a=ANY) { if ( a.on() ) return StringList(""); return StringList(""); }

	SymbolList& getStarSymbol() { return starSymbol; }

	virtual StringList BufferIndex(const ACSPortHole * port, const char * name, const char * offset) { if ( port || name || offset ) return StringList(""); return StringList(""); }

    // main routine.
    virtual int runIt() { return FALSE; }


};

#endif

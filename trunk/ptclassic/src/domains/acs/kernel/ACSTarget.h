// This may look like C code, but it is really -*- C++ -*-
#ifndef _ACSTarget_h
#define _ACSTarget_h

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


 Programmers:  Eric Pauer (Sanders), Christopher Hylands, Edward A. Lee
 Date of creation: 1/15/98
 Version: @(#)ACSTarget.h	1.13 09/08/99

 Basic target for Adaptive Computing Systems.
 This target provides a common base class for all the ACS targets 
 to inherit from.

***********************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "HLLTarget.h"
#include "FloatState.h"
#include "dataType.h"
#include "ACSKnownCategory.h"
#include "ACSCore.h"

static const int DEBUG_ACSTARGET=0;

// Defined in ACSDomain.cc
extern const char ACSdomainName[];

class ACSTarget : public HLLTarget {
public:
    ACSTarget(const char* name, const char* starclass, const char* desc,
            const char* category = NULL, const char* assocDomain = ACSdomainName);
    /* virtual */ Block* makeNew() const;

    /* virtual */ int run() { if ( isCG() ) return(HLLTarget::run()); else return(Target::run()); }   // FIXME

    ~ACSTarget();
    // Class identification.
    /*virtual*/ int isA(const char*) const;

    // Accessor for core category.
   const char* getCoreCategory() { return coreCategory; }

   // Allows Fixed-point simulation to change Plasma type.
   virtual DataType mapType(DataType);

    /* virtual */ void setup();

    /* virtual */ void wrapup();

    // determine if this is a simulation run or code-generation.
    virtual int isCG() { return (ACSKnownCategory::find(coreCategory))->isCG(); }

protected:
    // From SDF default Target
    StringState coreCategory;
    StringState logFile;

    // schedulePeriod for interface with a timed domain.
    FloatState schedulePeriod;

    // determines if this is a Fixed-point simulation.
    virtual int isFixedPoint() { return (ACSKnownCategory::find(coreCategory))->isFixedPoint(); }


};

#endif //_ACSTarget_h

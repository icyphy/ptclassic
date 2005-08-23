#ifndef _CGCostTarget_h
#define _CGCostTarget_h 1
/**********************************************************************
Version identification:
 $Id$

Copyright (c) %Q% The Regents of the University of California.
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

 Programmer:  Raza Ahmed
 Date of creation: 07/09/96

 Target for the generation of cost of individual stars in the galaxy.

***********************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "StringList.h"
#include "StringState.h"
#include "IntState.h"
#include "GalIter.h"
#include "SDFScheduler.h"
#include "CGTarget.h"

class CGCostTarget : public CGTarget, public Galaxy {
public:
        // constructor
        CGCostTarget(const char* nam, const char* startypt, const char* desc);

        /*virtual*/ Block* makeNew() const ;
	/*virtual*/ int run();
	/*virtual*/ void wrapup();

	// The following switch statement will find the right match in 
	// the data type for the block in the galaxy
	// The matched star will be then added to the passed in galaxy 
	// Then the two ports will be connected within the galaxy
        void selectConnectStarBlock(Galaxy* , MultiPortHole* , char*);

        // The following function will print the information about the
	// stars found in the galaxy
        void printGalaxy(Galaxy*);

        /*virtual*/ int isA(const char*) const;
private:
	// some private members to mask the pointer to the constant 
	// object to a pointer to a non-constant object
        Block* starPointer;     

};

#endif










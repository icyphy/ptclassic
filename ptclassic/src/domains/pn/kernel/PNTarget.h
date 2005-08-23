/* 
Copyright (c) 1990-1996 The Regents of the University of California.
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
/*  Version @(#)PNTarget.h	2.6 07/30/96
    Programmer:		T.M. Parks
    Date of creation:	2 April 1992
*/

#ifndef _PNTarget_h
#define _PNTarget_h

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"

class PNTarget : public Target
{
public:
    // Constructor.
    PNTarget();

    // Destructor.
    ~PNTarget();

    // Make a new PNTarget object.
    /* virtual */ Block* makeNew() const;

    // Initialization.
    /* virtual */ void setup();

    // Threads invoke the begin methods of the stars.
    /* virtual */ void begin() {}

    // End simulation.
    /* virtual */ void wrapup();
};

#endif

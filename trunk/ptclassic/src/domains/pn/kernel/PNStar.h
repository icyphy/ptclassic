/* 
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
*/
/*  Version $Id$
    Programmer:		T.M. Parks
    Date of creation:	6 February 1992

*/

#ifndef _MTDFStar_h
#define _MTDFStar_h

#ifdef __GNUG__
#pragma interface
#endif

#include "SDFStar.h"
#include "MTDFPortHole.h"
#include "TimeVal.h"

class MTDFThread;

class MTDFStar : public DataFlowStar
{
public:
    // Class identification.
    /* virtual */ int isA(const char* className) const;

    // Domain identification.
    /* virtual */ const char* domain() const;

    // Prepare for scheduling.
    /* virtual */ void initialize();

    // Execute the Star.
    /* virtual */ int run();

    // Associate a Thread with this Star.
    void setThread(MTDFThread&);

    // Thread associated with this Star.
    MTDFThread& thread();

    // Timing constraint specification.
    TimeVal lag;
    TimeVal period;

    void sleepUntil(TimeVal);

protected:
    MTDFThread* myThread;
};

#endif

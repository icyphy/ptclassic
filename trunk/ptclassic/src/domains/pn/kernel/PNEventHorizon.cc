/* 
Copyright (c) 1990-1993 The Regents of the University of California.
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
*/
/*  Version $Id$
    Author:	T. M. Parks
    Created:	20 January 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFEventHorizon.h"
#include "MTDFScheduler.h"

// --------------------MTDFtoUniversal--------------------

void MTDFtoUniversal::initialize()
{
    InMTDFPort::initialize();
    ToEventHorizon::initialize();
}

int MTDFtoUniversal::isItInput() const
{
    return ToEventHorizon::isItInput();
}

int MTDFtoUniversal::isItOutput() const
{
    return ToEventHorizon::isItOutput();
}

EventHorizon* MTDFtoUniversal::asEH()
{
    return this;
}

void MTDFtoUniversal::receiveData()
{
    InMTDFPort::receiveData();	// Block until data is available.
    tokenNew = TRUE;

    if (MTDFtoUniversal::isItInput())	// MTDF on the outside.
    {
	timeMark = outerSched()->now();
    }
    else		// MTDF on the inside.
    {
	// Undo the time increment at the end of the run.
	MTDFScheduler* sched = (MTDFScheduler*)innerSched();
	timeMark = sched->now() - sched->schedulePeriod;
    }

    transferData();
}

// -------------------MTDFfromUniversal-------------------

void MTDFfromUniversal::initialize()
{
    OutMTDFPort::initialize();
    FromEventHorizon::initialize();
}

int MTDFfromUniversal::isItInput() const
{
    return FromEventHorizon::isItInput();
}

int MTDFfromUniversal::isItOutput() const
{
    return FromEventHorizon::isItOutput();
}

EventHorizon* MTDFfromUniversal::asEH()
{
    return this;
}

void MTDFfromUniversal::sendData()
{
    // Transfer all available data, even if there is more than expected.
    transferData();
    while (tokenNew)
    {
	putData();
	transferData();
    }
}

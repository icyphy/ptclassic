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
    Author:	T.M. Parks
    Created:	7 February 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFStar.h"
#include "MTDFScheduler.h"
#include "MTDFThread.h"
#include "MTDFGeodesic.h"

extern const char MTDFdomainName[];

// Class identification.
ISA_FUNC(MTDFStar,DataFlowStar);

// Domain identification.
const char* MTDFStar::domain() const
{
    return MTDFdomainName;
}

// Prepare for scheduling.
void MTDFStar::initialize()
{
    DataFlowStar::initialize();
    if (isSource())
    {
        BlockPortIter next(*this);
        // Limit the capacity of Geodesics.
	for(int i = numberPorts(); i > 0; i--)
        {
	    PortHole& port = *next++;
            MTDFGeodesic& geodesic = *(MTDFGeodesic*)port.geo();
	    geodesic.setCapacity(port.numXfer());
        }
    }
}

// Execute the Star.
int MTDFStar::run()
{
    BlockPortIter next(*this);

    // Receive data for synchronous ports.
    for(int i = numberPorts(); i > 0; i--)
    {
	MTDFPortHole& port = *(MTDFPortHole*)next++;
	if(!port.isDynamic()) port.receiveData();
    }

    int status = Star::run();

    // Send data for synchronous ports, even on error.
    next.reset();
    for(i = numberPorts(); i > 0; i--)
    {
	MTDFPortHole& port = *(MTDFPortHole*)next++;
	if(!port.isDynamic()) port.sendData();
    }
    return status;
}


// Associate a Thread with this Star.
void MTDFStar::setThread(MTDFThread& t)
{
    myThread = &t;
}

// Thread associated with this Star.
MTDFThread& MTDFStar::thread()
{
    return *myThread;
}

void MTDFStar::sleepUntil(TimeVal wake)
{
    MTDFScheduler& sched = *(MTDFScheduler*)scheduler();
    thread().sleep(sched.delay(wake));
}

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
    Date of creation:	7 February 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MTDFStar.h"
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
        BlockPortIter portIter(*this);
        PortHole* port;

        // Limit the capacity of Geodesics.
        while((port = portIter++) != NULL)
        {
            MTDFGeodesic* geodesic = (MTDFGeodesic*)port->geo();
	    int capacity = (port->numXfer() > port->far()->numXfer()) ?
		port->numXfer() : port->far()->numXfer();
	    geodesic->setCapacity(capacity);
        }
    }
}

// Execute the Star.
int MTDFStar::run()
{
    return Star::run();
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

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
    Created:	7 February 1992
*/

static const char file_id[] = "$RCSfile$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "PNStar.h"

extern const char PNdomainName[];

// Class identification.
ISA_FUNC(PNStar,DataFlowStar);

// Domain identification.
const char* PNStar::domain() const
{
    return PNdomainName;
}

// Execute the Star.
int PNStar::run()
{
    BlockPortIter portIter(*this);
    DFPortHole* port;

    // Receive data for synchronous ports.
    while((port = (DFPortHole*)portIter++) != NULL)
    {
	if(!port->isDynamic()) port->receiveData();
    }

    int status = Star::run();

    // Send data for synchronous ports, even on error.
    portIter.reset();
    while((port = (DFPortHole*)portIter++) != NULL)
    {
	if(!port->isDynamic()) port->sendData();
    }
    return status;
}

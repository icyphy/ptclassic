static const char file_id[] = "XXXPortHole.cc";

/*  Version $Id$

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

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

    Code for domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "XXXPortHole.h"

// Class identification.
ISA_FUNC(XXXPortHole,PortHole);

// Input/output identification.
int InXXXPort::isItInput() const
{
    return TRUE;
}

// Input/output identification.
int OutXXXPort::isItOutput() const
{
    return TRUE;
}

// Input/output identification.
int MultiInXXXPort::isItInput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInXXXPort::newPort()
{
	LOG_NEW; PortHole& p = *new InXXXPort;
	return installPort(p);
}

// Input/output identification.
int MultiOutXXXPort::isItOutput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutXXXPort::newPort()
{
	LOG_NEW; PortHole& p = *new OutXXXPort;
	return installPort(p);
}

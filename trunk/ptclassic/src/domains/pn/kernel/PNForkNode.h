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

    Copyright 1992 The Regents of the University of California.
			All Rights Reserved.

    Programmer:		T.M. Parks
    Date of creation:	28 August 1992

    Auto-forking version of MTDFGeodesic.
*/

#ifndef _MTDFForkNode_h
#define _MTDFForkNode_h

#ifdef __GNUG__
#pragma interface
#endif

#include "MTDFGeodesic.h"
#include "AutoFork.h"

class MTDFForkNode : public MTDFGeodesic
{
public:
    // Class identification.
    virtual int isA(const char*) const;

    // Constructor.
    MTDFForkNode() : autoFork(*this) {}

    // Identify node as persistent.
    int isItPersistent () const;

    // Make a new source connection.
    PortHole* setSourcePort (GenericPort &, int delay = 0);

    // Make a new destination connection.
    PortHole* setDestPort (GenericPort &);

private:
    AutoFork autoFork;
};

#endif

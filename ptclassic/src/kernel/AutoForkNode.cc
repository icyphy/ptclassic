static const char file_id[] = "AutoForkNode.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: J. Buck
 Date of creation: 11/7/90

 This glass is a Geodesic that automatically generates auto-forks
 when attempts are made to connect multiple outputs to it.  It forbids
 the specification of multiple inputs.
*************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "AutoForkNode.h"

// this type is persistent
int AutoForkNode::isItPersistent () const {
	return TRUE;
}

// make a new source connection
PortHole* AutoForkNode::setSourcePort (GenericPort &sp, int numDelays,
				       const char* initDelayValues) {
	return af.setSource(sp, numDelays, initDelayValues);
}

// make a new destination connection, possibly autoforking
PortHole* AutoForkNode::setDestPort (GenericPort &gp) {
	return af.setDest(gp);
}

// class id
ISA_FUNC(AutoForkNode,Geodesic);
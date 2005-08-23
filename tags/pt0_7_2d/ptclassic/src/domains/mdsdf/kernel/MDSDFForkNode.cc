static const char file_id[] = "MDSDFForkNode.cc";
/**************************************************************************
Version identification:
@(#)MDSDFForkNode.cc	1.2 3/7/96

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

 Programmer: Mike J. Chen

 This class is an auto-forking version of MDSDFGeodesic.  It is to
 MDSDFGeodesic as AutoForkNode is to Geodesic.  A template could be
 used to generate both.

*************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "MDSDFForkNode.h"

// this type is persistent
int MDSDFForkNode::isItPersistent () const {
	return TRUE;
}

// make a new source connection
PortHole* MDSDFForkNode::setSourcePort(GenericPort &sp, int numDelays,
                                       const char* initDelayValues) 
{
	return af.setSource(sp, numDelays, initDelayValues);
}

// make a new destination connection.
PortHole* MDSDFForkNode::setDestPort (GenericPort &gp) {
	return af.setDest(gp, 0);
}

// class id
ISA_FUNC(MDSDFForkNode,MDSDFGeodesic);

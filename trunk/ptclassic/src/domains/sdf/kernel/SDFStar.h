#ifndef _SDFStar_h
#define _SDFStar_h 1
#ifdef __GNUG__
#pragma interface
#endif

/******************************************************************
Version identification:
$Id$

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
							COPYRIGHTENDKEY

 Programmer:  E. A. Lee, D. G. Messerschmitt, J. Buck
 Date of creation: 5/29/90

 SDFStar is a Star that runs under the SDF Scheduler.
 Formerly defined in Star.h

*******************************************************************/

#include "DataFlowStar.h"
#include "SDFPortHole.h"

	////////////////////////////////////
	// class SDFStar
	////////////////////////////////////

class SDFStar : public DataFlowStar  {
public:
	SDFStar() {}

	// identify self as SDF
	int isSDF() const;
	int isSDFinContext() const;

	// my domain
	const char* domain() const;

	// class identification
	int isA(const char*) const;

};

class SDFStarPortIter : public BlockPortIter {
public:
	SDFStarPortIter(SDFStar& s) : BlockPortIter(s) {}
	SDFPortHole* next() { return (SDFPortHole*)BlockPortIter::next();}
	SDFPortHole* operator++(POSTFIX_OP) { return next();}
};

#endif

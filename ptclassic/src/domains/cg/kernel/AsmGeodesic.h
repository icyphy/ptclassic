#ifndef _AsmGeodesic_h
#define _AsmGeodesic_h 1
/******************************************************************
Version identification:
$Id$

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

 Programmer: J. Buck
 Modified by: E. A. Lee

 geodesics for AsmCode stars.  In addition to the functions of CG
 geodesics, they manage memory allocation for buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "AsmPortHole.h"
#include "CGGeodesic.h"

class ProcMemory;

class AsmGeodesic : public CGGeodesic {
public:
	AsmGeodesic () : mem(0), addr(0) {}

	// class identification
	int isA(const char*) const;

	// Assign a memory and address to the geodesic
	void assignAddr(ProcMemory& m, unsigned a);

	// Return the address assigned to the geodesic.
	// if I am a fork destination, my address is that of my source.
	unsigned address() const;

	// Return a pointer to the memory assigned to the geodesic
	// if I am a fork destination, my memory is that of my source.

	ProcMemory* memory() const;
protected:
	// determine whether extra memory should be used to obtain
	// linear access with no wraparound.
	double wasteFactor() const;
private:
	int internalBufSize() const;
	ProcMemory* mem;
	unsigned addr;
};

#endif

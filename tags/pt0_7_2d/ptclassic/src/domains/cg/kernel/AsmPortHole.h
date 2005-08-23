#ifndef _AsmPortHole_h
#define _AsmPortHole_h 1
/******************************************************************
Version identification:
@(#)AsmPortHole.h	1.22	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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
 Modified by: E. A. Lee & Jose L. Pino

 These classes are portholes for stars that generate assembly language code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif
#include "CGPortHole.h"

// portholes for AsmCodeStars and derived stars
const bitWord PB_CIRC = 0x40;
const bitWord PB_SYMMETRIC = 0x200;
const bitWord PB_NOINIT	= 0x400;

extern const Attribute P_CIRC;
extern const Attribute P_SYMMETRIC;
extern const Attribute P_NONCIRC;
extern const Attribute P_NOINIT;

// attributes for code generation portholes

class ProcMemory;
class AsmGeodesic;

// PortHole class specific to assembly code generation.
// It contains methods for allocating memory for inputs and outputs.
class AsmPortHole : public CGPortHole {
	friend class AsmGeodesic;
public:
	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	AsmGeodesic& geo() const { return *(AsmGeodesic*)myGeodesic;}

	// Assign a memory address to the geodesic connected to this PortHole.
	void assignAddr(ProcMemory& m, unsigned a);

	// Return the base address of the memory allocated to the
	// geodesic connected to this PortHole.
	unsigned baseAddr() const;

	// Return the memory allocated to the
	// geodesic connected to this PortHole.
	ProcMemory* memory() const;

	// Return an int indicating the address with an offset for
	// the current access.
	virtual int location();

	// Return TRUE if a circular buffer access is ever required
	// for this buffer.  This was either specified by the programmer,
	// or required because the number of tokens produced or consumed
	// does not evenly divide the buffer size.
	int circAccess() const;

	// Return TRUE if an advance now will cause a wraparound.
	// This indicates that circular access should be used this time.
	int circAccessThisTime() const {
		return offset + numberTokens > bufSize();
	}

	// Initialize the offset member -- must be called after
	// any setSDFParams calls on ports take place (e.g. start
	// funcs in AsmStars).  This is handled by doing it from
	// CGTarget after everything has been init-ed.
	// it returns TRUE on success, else FALSE (0)
	virtual int initOffset();
};

class InAsmPort : public AsmPortHole {
public:
	int isItInput() const;
};

class OutAsmPort : public AsmPortHole {
public:
	int isItOutput() const;

};

class MultiAsmPort : public MultiCGPort {
public:
	MultiAsmPort() {}
	~MultiAsmPort() {}
};

class MultiInAsmPort : public MultiAsmPort {
public:
	int isItInput() const;

	// Add a new physical port to the MultiPortHole list
	PortHole& newPort();
};

class MultiOutAsmPort : public MultiAsmPort {
public:
	int isItOutput() const;
	// Add a new physical port to the MultiPortHole list
	PortHole& newPort();
};

#endif

#ifndef _VHDLBPortHole_h
#define _VHDLBPortHole_h 1
/******************************************************************
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

 Programmer: Edward A. Lee

 These classes are portholes for stars that generate VHDLB code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "PortHole.h"
#include "SDFPortHole.h"
#include "VHDLBGeodesic.h"

class VHDLBPortHole : public CGPortHole {
friend class VHDLBForkDestIter;
public:
	VHDLBPortHole() {}
	~VHDLBPortHole() {}

	VHDLBPortHole* getForkSrc() { return (VHDLBPortHole*) forkSrc; }

	// const version
	const VHDLBPortHole* getForkSrc() const {
		return (const VHDLBPortHole*) forkSrc;
	}

	// return the far port bypassing the fork stars
	VHDLBPortHole* realFarPort();
	// const version
	const VHDLBPortHole* realFarPort() const;

	// make a complete list of forkDests after resolving the indirect
	// list via fork stars
	void setupForkDests();

	void setGeoName(char* n);

	// Return the name of the geodesic connected to a port.
	// If the port is aliased from a galaxy port, return the name
	// of that galaxy port.  If it is a galaxy port, and is aliased
	// to a real port, return the name on the geodesic connected to
	// the real port. Append VHDLB delay notation when the port
	// is not aliased from a galaxy port and there is delay on the
	// geodesic.
	StringList getGeoReference() const;

	// Return the name on a geodesic
	const char* getGeoName() const;

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic has been
	// redefined to make myGeodesic of type VHDLFGeodesic.
	VHDLBGeodesic& geo() { return *(VHDLBGeodesic*)myGeodesic;}

	// const version
	const VHDLBGeodesic& geo() const {
		return *(const VHDLBGeodesic*)myGeodesic;}

private:
	SequentialList& myDest() { return forkDests; }
};

class InVHDLBPort : public VHDLBPortHole {
public:
	int isItInput() const;
};

class OutVHDLBPort: public VHDLBPortHole {
public:
	int isItOutput() const;
};

// is anything here?
class MultiVHDLBPort : public MultiCGPort {
public:
	int someFunc();
};

class MultiInVHDLBPort : public MultiVHDLBPort {
public:
	int isItInput() const;
	PortHole& newPort();
};

class MultiOutVHDLBPort : public MultiVHDLBPort {
public:
	int isItOutput() const;
	PortHole& newPort();
};

class VHDLBForkDestIter : private ListIter {
public:
	VHDLBForkDestIter(VHDLBPortHole* p) : ListIter(p->myDest()) {}
	VHDLBPortHole* next() { return (VHDLBPortHole*) ListIter::next(); }
	VHDLBPortHole* operator++ () { return next(); }
	ListIter::reset;
};
#endif

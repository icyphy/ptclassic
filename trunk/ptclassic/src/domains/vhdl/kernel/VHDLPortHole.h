#ifndef _VHDLPortHole_h
#define _VHDLPortHole_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: Edward A. Lee, Michael C. Williamson

 These classes are portholes for stars that generate VHDL code.  

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "VHDLGeodesic.h"
#include "CGPortHole.h"

class VHDLPortHole : public CGPortHole {
friend class VHDLForkDestIter;
public:
	VHDLPortHole() : bufName(0) {}
	~VHDLPortHole();

	void initialize();

	VHDLPortHole* getForkSrc() { return (VHDLPortHole*) forkSrc; }

	// const version
	const VHDLPortHole* getForkSrc() const {
		return (const VHDLPortHole*) forkSrc;
	}

	// return the far port bypassing the fork stars
	VHDLPortHole* realFarPort();
	// const version
	const VHDLPortHole* realFarPort() const;

	// make a complete list of forkDests after resolving the indirect
	// list via fork stars
	void setupForkDests();

	// name the porthole in the data structure.
	void setGeoName(char*);
	const char* getGeoName() const;

	// Return the geodesic connected to this PortHole.
	// This is typesafe ONLY when VHDL stars are used in the VHDL domain.
	// Will this be a problem?
	VHDLGeodesic& geo() { return *(VHDLGeodesic*)myGeodesic;}

	// const version
	const VHDLGeodesic& geo() const {
		return *(const VHDLGeodesic*)myGeodesic;}

        // Return the VHDL port direction corresponding to the port direction.
	StringList direction() const;

        // Return the VHDL datatype corresponding to the port type.
	StringList dataType() const;

private:
	char* bufName;		// set if no geodesic is assigned.

	SequentialList& myDest() { return forkDests; }

};

class InVHDLPort : public VHDLPortHole {
public:
	int isItInput() const;
};

class OutVHDLPort: public VHDLPortHole {
public:
	int isItOutput() const;
};

// is anything here?
class MultiVHDLPort : public MultiCGPort {
public:
	int someFunc();
};

class MultiInVHDLPort : public MultiVHDLPort {
public:
	int isItInput() const;
	PortHole& newPort();
};

class MultiOutVHDLPort : public MultiVHDLPort {
public:
	int isItOutput() const;
	PortHole& newPort();
};

class VHDLForkDestIter : private ListIter {
public:
	VHDLForkDestIter(VHDLPortHole* p) : ListIter(p->myDest()) {}
	VHDLPortHole* next() { return (VHDLPortHole*) ListIter::next(); }
	VHDLPortHole* operator++ (POSTFIX_OP) { return next(); }
	ListIter::reset;
};

#endif

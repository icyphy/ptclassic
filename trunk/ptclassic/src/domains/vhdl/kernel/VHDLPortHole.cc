static const char file_id[] = "VHDLPortHole.cc";
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
#pragma implementation
#endif

#include "VHDLPortHole.h"
#include "VHDLGeodesic.h"
#include "Error.h"

VHDLPortHole :: ~VHDLPortHole() {
	LOG_DEL; delete bufName;
}

void VHDLPortHole :: initialize() {
	CGPortHole :: initialize();
}

// setup VHDLForkDests
void VHDLPortHole :: setupForkDests() {
	SequentialList temp;
	temp.initialize();

	VHDLForkDestIter next(this);
	VHDLPortHole *outp, *inp;
	while ((outp = next++) != 0) {
		//  check wormhole boundary
		if (outp->far()->isItOutput()) continue;

		inp = outp->realFarPort();
		if (inp->fork()) temp.put(inp);
	}

	ListIter nextPort(temp);
	while ((inp = (VHDLPortHole*) nextPort++) != 0) {
		inp->setupForkDests();
		forkDests.remove(inp->far());
		VHDLForkDestIter realNext(inp);
		while ((outp = realNext++) != 0)
			forkDests.put(outp);
	}
}

// Need modification if we allow wormholes!!
VHDLPortHole* VHDLPortHole :: realFarPort() {
	VHDLPortHole* p = (VHDLPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

// const version: hate this duplication.
const VHDLPortHole* VHDLPortHole :: realFarPort() const {
	const VHDLPortHole* p = (const VHDLPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

void VHDLPortHole :: setGeoName(char* n) {
	if (myGeodesic == 0) bufName = n;
	else geo().setBufName(n);
}

const char* VHDLPortHole :: getGeoName() const {
	if (bufName) return bufName;
	return geo().getBufName();
}

// Return the VHDL port direction corresponding to the port direction.
StringList VHDLPortHole :: direction() const {
  StringList direction;
  int in, out;

  direction.initialize();
  in = this->isItInput();
  out = this->isItOutput();
  
  if (in && out) direction = "INOUT";
  else if (in) direction = "IN";
  else if (out) direction = "OUT";
  else Error::error(*this, " is neither input nor output");

  return direction;
}

// Return the VHDL datatype corresponding to the port type.
StringList VHDLPortHole :: dataType() const {
  StringList type;
  DataType dtyp = this->resolvedType();

  type.initialize();
  
  if (dtyp == INT) type << "INTEGER";
  else if (dtyp == COMPLEX) type << "complex";
  else type << "REAL";

  return type;
}

// Update the offset pointer to the queue of connected geodesic.
void VHDLPortHole :: updateOffset() {
  if (isItInput()) {
    geo().getTokens(numXfer());
  }
  else if (isItOutput()) {
    geo().putTokens(numXfer());
  }
}

// Get the offset pointer to the queue of connected geodesic.
int VHDLPortHole :: getOffset() {
  if (isItInput()) {
    return geo().nextGet();
  }
  else if (isItOutput()) {
    return geo().nextPut();
  }
  else {
    return 1;
  }
}

// Dummy
int MultiVHDLPort :: someFunc() { return 1; }

int InVHDLPort :: isItInput() const { return TRUE; }
int OutVHDLPort :: isItOutput() const { return TRUE; }
int MultiInVHDLPort :: isItInput() const { return TRUE; }
int MultiOutVHDLPort :: isItOutput() const { return TRUE; }

PortHole& MultiInVHDLPort :: newPort () {
        LOG_NEW; InVHDLPort* p = new InVHDLPort;
	p->setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(*p);
        return installPort(*p);
}

PortHole& MultiOutVHDLPort :: newPort () {
	LOG_NEW; OutVHDLPort* p = new OutVHDLPort;
	p->setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(*p);
        return installPort(*p);
}

static const char file_id[] = "CGCPortHole.cc";
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

 Programmer: S. Ha and E. A. Lee

 These classes are portholes for stars that generate C code.  

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "ConstIters.h"
#include "CGCPortHole.h"
#include "CGCGeodesic.h"
#include "SDFStar.h"
#include "CGCStar.h"
#include "Error.h"
#include "SimControl.h"

extern const Attribute P_GLOBAL = {PB_GLOBAL,0};
extern const Attribute P_LOCAL = {0,PB_GLOBAL};

CGCPortHole :: ~CGCPortHole() {
	LOG_DEL; delete bufName;
}

void CGCPortHole :: initialize() {
	CGPortHole :: initialize();

	// member initialize.
	myType = NA;
	hasStaticBuf = TRUE;
	asLinearBuf = TRUE;
	manualFlag = FALSE;
	maxBuf = 1;
	manualOffset = 0;
}

void CGCPortHole::setFlags() {
	if (isItOutput() && (embedded() || embedding())) {
		asLinearBuf = TRUE;
		hasStaticBuf = TRUE;
		return;
	}
	if (bufSize() % numXfer() != 0)
		asLinearBuf = FALSE;
	
	// BufferSize might not be set - bufSize class error abort run
	if (SimControl::haltRequested()) return;

	if (!bufSize()) {
	    Error::abortRun(*this," Buffer size has not been set.  Has the galaxy been scheduled?");
	    return;
	}

	if ((numXfer() * parentReps()) % bufSize() != 0)
		hasStaticBuf = FALSE; 
}

		////////////////////////////////////////
		// Buffer type determination routines
		////////////////////////////////////////

// Set the buffer type.
void CGCPortHole :: setBufferType() {
	if (isItOutput()) {
		CGCPortHole* farP = (CGCPortHole*) far();
		if (embedded()) myType = EMBEDDED;
		else if (farP && farP->embedded()) myType = EMBEDDED;
		else myType = OWNER;
	}
}

BufType CGCPortHole::bufType() const { 
	if (switched()) {
		CGCPortHole* cp = (CGCPortHole*) cgGeo().sourcePort();
		return cp->bufType();
	}
	if ((far() == 0) || atBoundary()) return myType;
	return isItOutput()? myType: realFarPort()->bufType(); 
}

		////////////////////////////////////////
		// Buffer size determination routines
		////////////////////////////////////////

// initialize the offset member. If there is no fork involved, output
// porthole start writing from offset 0, and the input porthole
// start reading from the (maxBuf - offset).
int CGCPortHole :: initOffset() {
	if (manualOffset) {
		offset = manualOffset;
		asLinearBuf = FALSE;
		hasStaticBuf = FALSE;
		return TRUE;
	}
	if (isItOutput()) {
		offset = numXfer() - 1;
		return TRUE;
	}
	int del = cgGeo().forkDelay();
	if (!del) del = numInitDelays();
	if (!del) offset = numXfer() - 1;
	else {
		offset = numXfer() - del - 1;
		if (offset < 0)  offset += maxBufReq();
	}
	if (del > maxBufReq()) {
		Error :: abortRun(*this, " delay is too large\n");
		return FALSE;
	}
	return TRUE;
}

// return the buffer requirements. Indicates whether static buffering
// is achieved or not.
// return 1 if on the boundary
int CGCPortHole :: maxBufReq() const {
	if (switched()) {
		CGCPortHole* cp = (CGCPortHole*) cgGeo().sourcePort();
		return cp->maxBufReq();
	}
	if ((far() == 0) || atBoundary()) return maxBuf;
	return isItOutput()? maxBuf: realFarPort()->maxBufReq();
}

// Decide the buffer size finally
// reserve the buffer for initial offsets which the farSide input port
// will read.
void CGCPortHole :: finalBufSize(int statBuf) {
	if (isItInput()) return;
	else if (far() == 0) {
		maxBuf = numXfer();
		return;
	}

	if (far()->isItOutput()) {
		maxBuf = localBufSize();
		return; // check boundary.
	}

	// Try best to realize Linear or static buffering.
	// Look at CGGeodesic.cc to see the actual code for buffer size
	// determination.
	if (usesOldValues()) {
		// if past values are used. give up linear buffering.
		asLinearBuf = FALSE;
		statBuf = 0;
	}
	geo().preferLinearBuf(statBuf);
	int reqSize = localBufSize();

	// check whether this size is set manually or not.
	// If yes, range check.
	if (manualFlag) {
		if (maxBuf < reqSize) {
			Error::warn(*this, "buffer request is too small.");
			maxBuf = reqSize;
		}
	} else { // static buffering option.
		if (asLinearBuf && (reqSize % numXfer() != 0)) {
			// for maximum chance of linear buffering.
			maxBuf = ((reqSize/numXfer())+1) * numXfer();
		} else {
			maxBuf = reqSize;
		}
	}

	if ((!statBuf) && (maxBuf > numXfer()))
		giveUpStatic();

	// set the flags
	setFlags();
	CGCPortHole* p = realFarPort();
	if (p->fork()) {
		// determine the maximum offset.
		ForkDestIter next(p);
		CGCPortHole* outp;
		while ((outp = next++) != 0) {
			CGCPortHole* inp = outp->realFarPort();
			if ((!statBuf) && (maxBuf > inp->numXfer()))
				inp->giveUpStatic();
			inp->setFlags();
			// access to the past Particles
			if (inp->usesOldValues() ||
				(inp->numInitDelays() % inp->numXfer() != 0))
				inp->asLinearBuf = FALSE;
		}
	} else {
		if ((!statBuf) && (maxBuf > p->numXfer()))
			p->giveUpStatic();
		p->setFlags();
		if (p->usesOldValues() ||
			(p->numInitDelays() % p->numXfer() != 0)) {
			p->asLinearBuf = FALSE;
		}
	}
}

// setup ForkDests
void CGCPortHole :: setupForkDests() {
	SequentialList temp;
	temp.initialize();

	ForkDestIter next(this);
	CGCPortHole *outp, *inp;
	while ((outp = next++) != 0) {
		//  check boundary
		if (!outp->far() || outp->far()->isItOutput()) continue;

		inp = outp->realFarPort();
		if (inp->fork()) temp.put(inp);
	}

	ListIter nextPort(temp);
	while ((inp = (CGCPortHole*) nextPort++) != 0) {
		inp->setupForkDests();
		forkDests.remove(inp->far());
		ForkDestIter realNext(inp);
		while ((outp = realNext++) != 0)
			forkDests.put(outp);
	}
}

CGCPortHole* CGCPortHole :: realFarPort() {
	CGCPortHole* p = (CGCPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

// const version: hate this duplication.
const CGCPortHole* CGCPortHole :: realFarPort() const {
	const CGCPortHole* p = (const CGCPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

void CGCPortHole :: setGeoName(char* n) {
	if (myGeodesic == 0) bufName = n;
	else geo().setBufName(n);
}

const char* CGCPortHole :: getGeoName() const {
	if (bufName) return bufName;
	return geo().getBufName();
}

// If type conversion between complex to float/int is required when
// it is an output.
int CGCPortHole :: isConverted(){
	if (isItInput()) return converted;
	else if (strcmp(type(),resolvedType()) == 0) converted = FALSE;
	else if (strcmp(type(), ANYTYPE) == 0) converted = FALSE;
	else if ((strcmp(type(),COMPLEX) == 0) || 
	    (strcmp(type(),"COMPLEXARRAY") == 0)) converted = 1;
	else if ((strcmp(resolvedType(),COMPLEX) == 0) || 
	    (strcmp(resolvedType(),"COMPLEXARRAY") == 0)) converted = -1;
	else converted = FALSE;

	return converted;
}

// return the precision for a port of type FIX that was assigned
// with the setPrecision() method
Precision CGCPortHole::getAssignedPrecision() const
{
	MultiCGCPort* myMultiPort  = (MultiCGCPort*)getMyMultiPortHole();
	MultiCGCPort* farMultiPort = (MultiCGCPort*)realFarPort()->getMyMultiPortHole();

	// return the precision of this port or of the associated multiport
	if (prec.isValid())
		return prec;

   else if (myMultiPort && myMultiPort->validPrecision())
		return myMultiPort->precision();

	else {
		// if the precision is not set explicitly, return the precision
		// of the connected port

		if (realFarPort()->validPrecision())
			return realFarPort()->precision();

	   else if (farMultiPort && farMultiPort->validPrecision())
			return farMultiPort->precision();
	}

	// return empty precision structure
	return Precision();
}

// overloaded % operators used to obtain the fixed-point precision
// of an element of the porthole buffer
CGCPrecisionHolder CGCPortHole::operator % (int offset) const
{
	char buffer[16];
	sprintf(buffer, "%d", offset);
	return *this % buffer;
}

CGCPrecisionHolder CGCPortHole::operator % (const char* symbolic_offset) const
{
	// function declared in CGCStar.cc
	extern const char* sanitize(const char*);

	CGCStar* star = (CGCStar*)this->parent();

	// if this is a port that uses precision variables,
	// produce a symbolic precision representation that contains
	// references to the entry in the array of fix_prec variables
	// that belongs to the specified offset

	if ((attributes() & AB_VARPREC) ||
	    (realFarPort()->attributes() & AB_VARPREC)) {

	    const CGCPortHole* this_side =
		(const CGCPortHole*)((attributes() & AB_VARPREC) ? this : realFarPort());

	    StringList ref;
	    ref << this_side->getGeoName() << 'p';

	    // Use array notation for large buffers and for embedded buffers
	    // which are referenced through a pointer.

	    if (bufSize() > 1 || bufType() == EMBEDDED)
	    {
		ref << "[(";

		if (staticBuf()) ref << bufPos();
		else ref << sanitize(star->starSymbol.lookup(name()));
		if (strcmp(symbolic_offset,"0"))
			ref << "-(" << symbolic_offset << ')';
		if (!linearBuf())	// use circular addressing
		{
		    ref << '+' << bufSize() << ")%" << bufSize();
		}
		else ref << ')';

		ref << ']';
	    }

	    StringList sym_len, sym_intb;
	    sym_len  << (const char*)ref << ".len";
	    sym_intb << (const char*)ref << ".intb";

	    // determine the non-symbolic initial value of the precision
	    // variable that was assigned by the user
	    Precision initial_prec = getAssignedPrecision();

	    CGCPrecisionHolder ph(
		initial_prec.len(),initial_prec.intb(), sym_len,sym_intb);

	    return ph;
	}
	else
	{
	    // otherwise pass the assigned precision
	    return CGCPrecisionHolder(getAssignedPrecision());
	}
}

// Dummy
int MultiCGCPort :: someFunc() { return 1; }

int InCGCPort :: isItInput() const { return TRUE; }
int OutCGCPort :: isItOutput() const { return TRUE; }
int MultiInCGCPort :: isItInput() const { return TRUE; }
int MultiOutCGCPort :: isItOutput() const { return TRUE; }

// create new portholes from a multiport;
// note that the precision of the multiport is not passed to the new port
// since it is usually not defined at this time;
// therefore the CGCPortHole::precision() method must eventually check the
// precision of the associated multiport.

PortHole& MultiInCGCPort :: newPort () {
        LOG_NEW; InCGCPort* p = new InCGCPort;
	p->setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(*p);
        return installPort(*p);
}

PortHole& MultiOutCGCPort :: newPort () {
	LOG_NEW; OutCGCPort* p = new OutCGCPort;
	p->setSDFParams(numberTokens,numberTokens-1);
	forkProcessing(*p);
        return installPort(*p);
}

// return the precision for a multi port of type FIX;
// if the precision has not been explicitly set via the setPrecision()
// method, a combination of the precisions of the connected portholes
// is returned
Precision MultiCGCPort::precision() const
{
	if (prec.isValid())
		return prec;
	else {
		CMPHIter nextPort(*(const MultiCGCPort*)this);
		const CGCPortHole* port;

		// use the += operator of class Precision to combine the
		// precisions of the connected ports;
		// it returns a precision whose integer and fraction parts
		// are large enough to hold the fix values of both ports,
		// which is exactly what we want
		
		Precision p;
		while ((port = (const CGCPortHole*)nextPort++) != NULL) {
			p += port->precision();
		}
		return prec;
	}
}


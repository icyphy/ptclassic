static const char file_id[] = "ACSPortHole.cc";

/*  Version @(#)ACSPortHole.cc	1.12 09/10/99

@Copyright (c) 1998 The Regents of the University of California.
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

    Programmer:		T.M. Parks
    Date of creation:	5 January 1992

    Code for domain-specific PortHole classes.
*/

#ifdef __GNUG__
#pragma implementation
#endif
#include <stdio.h>              // sprintf()
#include "ConstIters.h"
#include "ACSPortHole.h"
#include "ACSTarget.h"
#include "ACSGeodesic.h"
#include "Plasma.h"
#include "CircularBuffer.h"

// Class identification.
//ISA_FUNC(ACSPortHole,PortHole);

// Input/output identification.
int InACSPort::isItInput() const
{
    return TRUE;
}

// Input/output identification.
int OutACSPort::isItOutput() const
{
    return TRUE;
}

// Input/output identification.
int MultiInACSPort::isItInput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInACSPort::newPort()
{
	LOG_NEW; PortHole& p = *new InACSPort;
	return installPort(p);
}

// Input/output identification.
int MultiOutACSPort::isItOutput() const
{
    return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutACSPort::newPort()
{
	LOG_NEW; PortHole& p = *new OutACSPort;
	return installPort(p);
}

// Used for simulation particle I/O.
void InACSPort :: receiveData () { getParticle();}

void OutACSPort :: receiveData () { clearParticle();}

void OutACSPort :: sendData () { putParticle();}

// Allows Target to change Plasma type for Fixed-Point simulation.
int ACSPortHole :: allocatePlasma() {
	ACSTarget* myTarget = (ACSTarget*)(parent()->target());
	myPlasma = Plasma::getPlasma(myTarget->mapType(resolvedType()));
	if (!myPlasma) {
		Error::abortRun(*this, "ACSPortHole::allocatePlasma failed");
	}
	return (myPlasma != 0);
}

ACSPortHole :: ~ACSPortHole() {
	LOG_DEL; delete [] bufName;
}

void ACSPortHole :: initialize() {
	if (!setResolvedType ()) {
		Error::abortRun (*this, "can't determine DataType");
		return;
	}
	if (!allocatePlasma()) return;
	// allocate buffer
	allocateBuffer ();

	// initialize buffer
	for(int i = myBuffer->size(); i>0; i--) {
		Particle** p = myBuffer->next();
		// Initialize particles on the buffer, so when we
		// restart they do not contain old data
		// fill in any empty holes (which could be caused by
		// errors of some kinds in previous runs).
		if (*p)	(*p)->initialize();
		else *p = myPlasma->get();
	}
	// If this is an output PortHole (or connected to an
	// input porthole), reset myGeodesic
	if (far() && myGeodesic && (isItOutput() || (!asEH() && atBoundary())))
		myGeodesic->initialize();

	// member initialize. same values as constructor.
	maxBuf = 1;
	manualFlag = FALSE;
	hasStaticBuf = TRUE;
	asLinearBuf = TRUE;
	myType = NA;
	converted = FALSE;
	delete [] bufName;
	bufName = 0;
	manualOffset = 0;
}

// return the buffer requirements. Indicates whether static buffering
// is achieved or not.
// return 1 if on the boundary
int ACSPortHole :: maxBufReq() const {
	if (switched()) {
		ACSPortHole* cp = (ACSPortHole*) cgGeo().sourcePort();
		return cp->maxBufReq();
	}
	if (atBoundary()) return maxBuf;
	return isItOutput()? maxBuf: realFarPort()->maxBufReq();
}

// overloaded % operators used to obtain the fixed-point precision
// of an element of the porthole buffer
ACSPrecisionHolder ACSPortHole::operator ^ (int offset) const
{
	char buffer[16];
	sprintf(buffer, "%d", offset);
	return *this ^ buffer;
}

ACSPrecisionHolder ACSPortHole::operator ^ (const char* symbolic_offset) const
{
	ACSStar* star = (ACSStar*)this->parent();

	// if this is a port that uses precision variables,
	// produce a symbolic precision representation that contains
	// references to the entry in the array of fix_prec variables
	// that belongs to the specified offset

	if ((attributes() & AB_VARPREC) ||
	    (realFarPort()->attributes() & AB_VARPREC)) {

	    const ACSPortHole* this_side =
		(const ACSPortHole*)((attributes() & AB_VARPREC) ? this : realFarPort());

	    StringList ref;
	    ref << this_side->getGeoName() << 'p';

	    // Use array notation for large buffers and for embedded buffers
	    // which are referenced through a pointer.

	    if (bufSize() > 1 || bufType() == EMBEDDED)
	    {
		ref << star->BufferIndex(this, (star->getStarSymbol()).lookup(name()), symbolic_offset);
	    }

	    StringList sym_len, sym_intb;
	    sym_len  << (const char*)ref << ".len";
	    sym_intb << (const char*)ref << ".intb";

	    // determine the non-symbolic initial value of the precision
	    // variable that was assigned by the user
	    Precision initial_prec = getAssignedPrecision();

	    ACSPrecisionHolder ph(
		initial_prec.len(),initial_prec.intb(), sym_len,sym_intb);

	    return ph;
	}
	else
	{
	    // otherwise pass the assigned precision
	    return ACSPrecisionHolder(getAssignedPrecision());
	}
}

ACSPortHole* ACSPortHole :: realFarPort() {
	ACSPortHole* p = (ACSPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

BufType ACSPortHole::bufType() const { 
	if (switched()) {
		ACSPortHole* cp = (ACSPortHole*) cgGeo().sourcePort();
		return cp->bufType();
	}
	if ((far() == 0) || atBoundary()) return myType;
	return isItOutput()? myType: realFarPort()->bufType(); 
}

// return the precision for a port of type FIX that was assigned
// with the setPrecision() method
Precision ACSPortHole::getAssignedPrecision() const
{
	MultiACSPort* myMultiPort  = (MultiACSPort*)getMyMultiPortHole();
	MultiACSPort* farMultiPort = (MultiACSPort*)realFarPort()->getMyMultiPortHole();

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

const char* ACSPortHole :: getGeoName() const {
	if (bufName) return bufName;
	return geo().getBufName();
}

const ACSPortHole* ACSPortHole :: realFarPort() const {
	ACSPortHole* p = (ACSPortHole*) far();
	if (p->getForkSrc()) return p->getForkSrc()->realFarPort();
	return p;
}

// return the precision for a multi port of type FIX;
// if the precision has not been explicitly set via the setPrecision()
// method, a combination of the precisions of the connected portholes
// is returned
Precision MultiACSPort::precision() const
{
	if (prec.isValid())
		return prec;
	else {
		CMPHIter nextPort(*(const MultiACSPort*)this);
		const ACSPortHole* port;

		// use the += operator of class Precision to combine the
		// precisions of the connected ports;
		// it returns a precision whose integer and fraction parts
		// are large enough to hold the fix values of both ports,
		// which is exactly what we want
		
		Precision p;
		while ((port = (const ACSPortHole*)nextPort++) != NULL) {
			p += port->precision();
		}
		return prec;
	}
}

void ACSPortHole :: setGeoName(const char* n) {
	delete [] bufName;
	if (myGeodesic == 0) bufName = savestring(n);
	else geo().setBufName(n);
}

// setup ForkDests
void ACSPortHole :: setupForkDests() {
	SequentialList temp;
	temp.initialize();

	ACSForkDestIter next(this);
	ACSPortHole *outp, *inp;
	while ((outp = next++) != 0) {
		// check boundary
		if (!outp->far() || outp->far()->isItOutput()) continue;

		inp = outp->realFarPort();
		if (inp->fork()) temp.put(inp);
	}

	ListIter nextPort(temp);
	while ((inp = (ACSPortHole*) nextPort++) != 0) {
		inp->setupForkDests();
		forkDests.remove(inp->far());
		ACSForkDestIter realNext(inp);
		while ((outp = realNext++) != 0)
			forkDests.put(outp);
	}
}

void ACSPortHole::setFlags() {
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
void ACSPortHole :: setBufferType() {
	if (isItOutput()) {
		ACSPortHole* farP = (ACSPortHole*) far();
		if (embedded()) myType = EMBEDDED;
		else if (farP && farP->embedded()) myType = EMBEDDED;
		else myType = OWNER;
	}
}

// Decide the buffer size finally
// reserve the buffer for initial offsets which the farSide input port
// will read.
void ACSPortHole :: finalBufSize(int statBuf) {
	if (isItInput())
		return;
	else if (far() == 0) {
		maxBuf = numXfer();
		return;
	}

	// check wormhole boundary
	if (far()->isItOutput()) {
		maxBuf = localBufSize();
		return;
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
	ACSPortHole* p = realFarPort();
	if (p->fork()) {
		// determine the maximum offset.
		ACSForkDestIter next(p);
		ACSPortHole* outp;
		while ((outp = next++) != 0) {
			ACSPortHole* inp = outp->realFarPort();
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
		////////////////////////////////////////
		// Buffer size determination routines
		////////////////////////////////////////

// initialize the offset member. If there is no fork involved, output
// porthole start writing from offset 0, and the input porthole
// start reading from the (maxBuf - offset).
int ACSPortHole :: initOffset() {
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

/* int ACSPortHole :: localBufSize() const {
	ACSGeodesic* g = (ACSGeodesic*)myGeodesic;
	if ( atBoundary() ) {
		return (parentReps()*numberTokens);
	} else { 
		return g->localBufSize();
	}
} */


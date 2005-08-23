/**************************************************************************
Version identification:
@(#)CGPortHole.h	1.21	3/2/96

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

 Programmer:  J. Buck, E. A. Lee

This file contains definitions of CG-specific PortHole classes.

******************************************************************/
#ifndef _CGPortHole_h
#define _CGPortHole_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DynDFPortHole.h"

class CGGeodesic;

/*****************************************************************
CG: Code generation

These PortHoles are much like SDF PortHoles; both have the common
baseclass DFPortHole.

****************************************************************/

	//////////////////////////////////////////
	// class CGPortHole
	//////////////////////////////////////////

// Contains all the special features required for
// synchronous dataflow and boolean dataflow (CG)

class CGPortHole : public DynDFPortHole {
	friend class CGGeodesic;
public:
	CGPortHole();
	~CGPortHole();

	// Services of DFPortHole and BDFPortHole that are used often:
	// setPort(char* portName, Block* parent, dataType type,
	//	unsigned numTokens, unsigned delay)
	// setSDFParams(unsigned numTokens, unsigned delay);
	// setBDFParams(...)

	// Return the geodesic connected to this PortHole.
	// This is typesafe because allocateGeodesic
	// makes myGeodesic of this type.
	CGGeodesic& cgGeo() const { return *(CGGeodesic*)myGeodesic;}

	// switch myGeodesic pointer to the argument Geodesic
	// If the geodesic is switched, no longer need to allocate the
	// memory for this port -> that means the argument geodesic should
	// be allocated beforehand.
	// This method is virtual since we may need to save the original
	// geodesic in other domains.
	virtual void switchGeo(Geodesic* g) { switchFlag = TRUE;
				      myGeodesic = g; }
	virtual void revertGeo(Geodesic* g) { switchFlag = FALSE;
					myGeodesic = g; }
	int switched() const { return switchFlag; }

	// Return the size of the buffer connected to this PortHole.
	virtual int bufSize() const;

	// Return the size of the "local buffer" connected to this
	// PortHole.  This returns zero for cases where no separate
	// buffer is allocated, e.g. fork outputs (all destinations
	// of the fork share the same buffer, whose size is returned
	// by bufSize).
	virtual int localBufSize() const;

	// return the offset position in the buffer.
	unsigned bufPos() const { return offset;}

	// Advance the offset by the number of tokens produced or
	// consumed in this PortHole when the Star fires.
	virtual void advance();

	// The buffer assigned to this porthole is embedded in a buffer:
	// ex) input.embed(output) means that the buffer of the input
	// porthole contains the buffer of the output porthole.
	void embed(CGPortHole& p, int i = DF_NONE) {
		p.embeddedPort = this; 
		embeddingFlag = TRUE;
		p.embeddedLoc = i; }
	CGPortHole* embedded() { return embeddedPort; }
	int embedding() { return embeddingFlag; }

	// return where embedded. return DF_NONE if not decided yet.
	int whereEmbedded() { return embeddedLoc; }
	void embedHere(int i) { embeddedLoc = i; }

	// return true if I am a fork input
	int fork() const { return forkDests.size();}

	// Manage fork destinations
	inline int removeForkDest(CGPortHole* p, int cgPortHoleFlag = TRUE) {
		if (cgPortHoleFlag) portHoleForkDests.remove(p);
		return forkDests.remove(p);
	}
	inline void putForkDest(CGPortHole* p, int cgPortHoleFlag = TRUE) {
		if (cgPortHoleFlag) portHoleForkDests.put(p);
		forkDests.put(p);
	}

	// set a fork source
	void setForkSource(CGPortHole* p, int cgPortHoleFlag = TRUE);

	// functions for moving data across wormhole boundaries
	void forceSendData() { putParticle(); }
	void forceGrabData() { getParticle(); }

protected:
	int offset;

	// Stuff to support fork buffers
	SequentialList forkDests;		// CGPortHoles + MultiCGPorts
	SequentialList portHoleForkDests;	// CGPortHoles only
	CGPortHole* forkSrc;

	// Where I am embedded.
	CGPortHole* embeddedPort;
	int embeddedLoc;
	// Am I embedding ports?
	int embeddingFlag;

private:
	// If the geodesic is switched, return TRUE;
	int switchFlag;
};

	///////////////////////////////////////////
	// class InCGPort
	//////////////////////////////////////////

class InCGPort : public CGPortHole
{
public:
	int isItInput () const ;		// { return TRUE; }
};

	////////////////////////////////////////////
	// class OutCGPort
	////////////////////////////////////////////

class OutCGPort : public CGPortHole
{
public:
	int isItOutput () const;		// { return TRUE; }
};

	//////////////////////////////////////////
	// class MultiCGPort
	//////////////////////////////////////////
 
// Synchronous dataflow MultiPortHole for code generation
// FIXME: by deriving from MultiDFPort we only support
// setSDFParams, not full set.

class MultiCGPort : public MultiDFPort {
public:
	MultiCGPort() : forkSrc(0) {}
	~MultiCGPort();

	void setForkBuf(CGPortHole& p) { forkSrc = &p; }

	// this function should be called by installPort for
	// all derived output multiporthole classes.
	void forkProcessing(CGPortHole&);

	// Services of MultiSDFPort that are used often:
	// setPort(const char* portName, Block* parent, dataType type,
	//	unsigned numTokens)

protected:
	CGPortHole* forkSrc;
};

	//////////////////////////////////////////
	// class MultiInCGPort
	//////////////////////////////////////////
	
// MultiInCGPort is an CG input MultiPortHole
 
class MultiInCGPort : public MultiCGPort {
public:
	int isItInput () const;			// { return TRUE; }

	// Add a new physical port to the MultiPortHole list
	PortHole& newPort();
};
 
 
	//////////////////////////////////////////
	// class MultiOutCGPort
	//////////////////////////////////////////

// MultiOutCGPort is an CG output MultiPortHole  

class MultiOutCGPort : public MultiCGPort {     
public:
	int isItOutput () const;		// { return TRUE; }

	// Add a new physical port to the MultiPortHole list
	PortHole& newPort();
};

#endif

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee

This file contains definitions of CG-specific PortHole classes.

******************************************************************/
#ifndef _CGConnect_h
#define _CGConnect_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Connect.h"
#include "SDFConnect.h"

class CGGeodesic;

/*****************************************************************
CG: Code generation

These PortHoles are much like SDF PortHoles, from which they are derived.
****************************************************************/

        //////////////////////////////////////////
        // class CGPortHole
        //////////////////////////////////////////

// Contains all the special features required for
//   synchronous dataflow (CG)

class CGPortHole : public SDFPortHole {
	friend class CGGeodesic;
public:
	CGPortHole();
	~CGPortHole();

	// Services of SDFPortHole that are used often:
	// setPort(char* portName, Block* parent, dataType type,
	//	unsigned numTokens, unsigned delay)
	// setSDFParams(unsigned numTokens, unsigned delay);

	// Allocate a geodesic and give it a name
	virtual Geodesic* allocateGeodesic();

	// Return the geodesic connected to this PortHole.
        // This is typesafe because allocateGeodesic
        // makes myGeodesic of this type.
        CGGeodesic& cgGeo() const { return *(CGGeodesic*)myGeodesic;}

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
	void embed(CGPortHole& p) { p.embeddedPort = this; }
	CGPortHole* embedded() { return embeddedPort; }

	// return true if I am a fork input
	int fork() const { return forkDests.size();}

	// set a fork source
        void setForkSource(CGPortHole* p);

	// functions for moving data across wormhole boundaries
	void forceSendData() { putParticle();}
	void forceGrabData() { getParticle();}

protected:
	int offset;
	// Stuff to support fork buffers
	SequentialList forkDests;
	CGPortHole* forkSrc;
	
	// Where I am embedded.
	CGPortHole* embeddedPort;
};

	///////////////////////////////////////////
	// class InCGPort
	//////////////////////////////////////////

class InCGPort : public CGPortHole
{
public:
	int isItInput () const ; // {return TRUE; }
};

	////////////////////////////////////////////
	// class OutCGPort
	////////////////////////////////////////////

class OutCGPort : public CGPortHole
{
public:
        int isItOutput () const; // {return TRUE; }
};

        //////////////////////////////////////////
        // class MultiCGPort
        //////////////////////////////////////////
 
// Synchronous dataflow MultiPortHole for code generation
 
class MultiCGPort : public MultiSDFPort {
protected:
	CGPortHole* forkSrc;
public:
	MultiCGPort() : forkSrc(0) {}
	~MultiCGPort();

	void setForkBuf(CGPortHole& p) { forkSrc = &p;}

	// this function should be called by installPort for
	// all derived output multiporthole classes.
	void forkProcessing(CGPortHole&);

	// Services of MultiSDFPort that are used often:
	// setPort(const char* portName, Block* parent, dataType type,
	//	unsigned numTokens)
};

        //////////////////////////////////////////
        // class MultiInCGPort
        //////////////////////////////////////////
        
// MultiInCGPort is an CG input MultiPortHole
 
class MultiInCGPort : public MultiCGPort {
public:
        int isItInput () const; // {return TRUE; }

	// Add a new physical port to the MultiPortHole list
	PortHole& newPort();
};
 
 
        //////////////////////////////////////////
        // class MultiOutCGPort
        //////////////////////////////////////////

// MultiOutCGPort is an CG output MultiPortHole  

class MultiOutCGPort : public MultiCGPort {     
public:
        int isItOutput () const; // {return TRUE; }

	// Add a new physical port to the MultiPortHole list
	PortHole& newPort();
};

#endif

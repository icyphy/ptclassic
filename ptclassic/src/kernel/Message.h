#ifndef _Packet_h
#define  _Packet_h 1

#ifdef __GNUG__
#pragma once
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 2/20/91

 This file defines the Packet interface of Ptolemy.  A Packet is
 an envelope for passing objects of type PacketData around.  A
 PacketSample is a type of Particle that transports Packets.

**************************************************************************/
#include "Particle.h"
#include "StringList.h"

extern const dataType PACKET;

class PacketData {
	friend class Packet;
private:
	int refCount;
protected:
	int errorConvert(const char*) const;
public:
	// constructor.  Reference count is zero.
	PacketData() : refCount(0) {}

	// destructor.
	virtual ~PacketData();

	// functions that may be specified by specific packets.
	virtual int asInt() const;
	virtual float asFloat() const;
	virtual Complex asComplex() const;

	// type of the PacketData.  When overriding, be SURE
	// that the prototype matches (don't forget the const keywords)
	virtual const char* dataType() const;

	// specify how to print the packet.
	virtual StringList print() const;

	// clone the packet.  MUST BE REDEFINED by each derived class!
	// don't forget the "const" keyword when you do so!
	virtual PacketData* clone() const;
};

// A Packet is simply a way of making a single PacketData look like
// multiple objects.

class Packet {
private:
	static PacketData dummyPacket;

	// a pointer to my real data
	PacketData* d;

	// bookkeeping function to zap the PacketData when done
	void unlinkData();
public:
	// constructor
	Packet(PacketData& dat = dummyPacket) {
		d = &dat;
		d->refCount++;
	}
	// copy constructor
	Packet(const Packet& p) {
		d = p.d;
		d->refCount++;
	}
	// assignment operator
	Packet& operator=(const Packet& p) {
		p.d->refCount++;
		unlinkData();
		d = p.d;
	}
	// destructor.  Wipe out the PacketData when the last
	// link is removed.
	~Packet() {
		unlinkData();
	}
	// dataType() : pass through
	const char* dataType() const { return d->dataType();}

	// type check: checks PacketData type
	int typeCheck(const char* type) const {
		return strcmp(dataType(), type) == 0;
	}

	// type error message generation
	// the pointer points to a static buffer!
	const char* typeError(const char* expected) const;

	// interfaces to PacketData functions
	int asInt() const { return d->asInt();}
	float asFloat() const { return d->asFloat();}
	Complex asComplex() const { return d->asComplex();}
	StringList print() const { return d->print();}

	// we can get a pointer to the PacketData.  It's a const
	// pointer so it cannot be used to alter the PacketData
	// (since there may be other references to it).
	const PacketData* myData() const { return d;}

	// produce a writable copy of the packetData.  side effect --
	// contents of Packet are changed to dummyPacket.
	PacketData* writableCopy();
};

// A Particle class to transmit Packets
class PacketSample : public Particle {
public:
	dataType readType() const;

	operator int () const;
	operator float () const;
	operator Complex () const;
	StringList print() const;

	void getPacket (Packet& p);

	// fill in remaining functions for Particle classes

	PacketSample(const Packet& p) : data(p) {}
	PacketSample() {}
	void initialize();

	// load with data -- these cause errors except for a Packet argument.

	void operator << (int i);
	void operator << (float f);
	void operator << (Complex& c);
	void operator << (const Packet& p);

	// particle copy
	Particle& operator = (const Particle& p);

	// clone, useNew, die analogous to other particles.
	// packetPlasma is of type Plasma.

	Particle* clone();

	Particle* useNew();
	void die();

private:
	void errorAssign(const char*) const;
	Packet data;
};

#endif

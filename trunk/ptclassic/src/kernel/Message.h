#ifndef _Packet_h
#define  _Packet_h 1

#ifdef __GNUG__
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

extern const DataType PACKET;

class PacketData {
	friend class Packet;
private:
	// we use indirection for the reference count so it can be
	// manipulated even for a const PacketData object.
	int* refCount;
protected:
	int errorConvert(const char*) const;
public:
	// constructor.  Reference count is zero.
	PacketData() {
		INC_LOG_NEW; refCount = new int; *refCount = 0;
	}

	// copy constructor.  Copy always has a zero ref count.
	// arg is ignored since all objects are the same.  The
	// default copy constructor cannot be used because we must
	// have a separate refCount field.
	PacketData(const PacketData&) {
		INC_LOG_NEW; refCount = new int; *refCount = 0;
	}

	// destructor.
	virtual ~PacketData();

	// functions that may be specified by specific packets.
	virtual int asInt() const;
	virtual double asFloat() const;
	virtual Complex asComplex() const;

	// type of the PacketData.  When overriding, be SURE
	// that the prototype matches (don't forget the const keywords)
	virtual const char* dataType() const;

	// specify how to print the packet.
	virtual StringList print() const;

	// clone the packet.  MUST BE REDEFINED by each derived class!
	// don't forget the "const" keyword when you do so!
	virtual PacketData* clone() const;

	// type checking: isA returns true if given the name of
	// the class or the name of any baseclass.  Exception:
	// the baseclass function returns FALSE to everything
	// (as it has no data at all).
	virtual int isA(const char*) const;
};

#include "isa.h"

// A Packet is simply a way of making a single PacketData look like
// multiple objects.

class Packet {
private:
	static PacketData dummyPacket;

	// a pointer to my real data
	PacketData* d;

	// manipulate the reference count.  These MUST be used
	// properly.
	void incCount() const { (*d->refCount)++;}
	void decCount() const { (*d->refCount)--;}

	int refCount() const { return *d->refCount;}

	// bookkeeping function to zap the PacketData when done
	void unlinkData();

public:
	// constructor: by default, point to dummyPacket.
	// dummyPacket is special, doesn't bother with ref counts.
	Packet() : d(&dummyPacket) {}

	Packet(PacketData& dat) : d(&dat) {
		incCount();
	}

	// copy constructor
	Packet(const Packet& p) {
		d = p.d;
		incCount();
	}
	// assignment operator
	Packet& operator=(const Packet& p) {
		p.incCount();
		unlinkData();
		d = p.d;
		return *this;
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
		return d->isA(type);
	}

	// type error message generation
	// the pointer points to a static buffer!
	const char* typeError(const char* expected) const;

	// interfaces to PacketData functions
	int asInt() const { return d->asInt();}
	double asFloat() const { return d->asFloat();}
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
	DataType readType() const;

	operator int () const;
	operator float () const;
	operator double () const;
	operator Complex () const;
	StringList print() const;

	void getPacket (Packet& p);
	void accessPacket (Packet& p) const;

	// fill in remaining functions for Particle classes

	PacketSample(const Packet& p) : data(p) {}
	PacketSample() {}
	void initialize();

	// load with data -- these cause errors except for a Packet argument.

	void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);
	void operator << (const Packet& p);

	// particle copy
	Particle& operator = (const Particle& p);

	// compare particles
	int operator == (const Particle&);

	// clone, useNew, die analogous to other particles.
	// packetPlasma is of type Plasma.

	Particle* clone();

	Particle* useNew();
	void die();

private:
	void errorAssign(const char*) const;
	Packet data;
};

// function and macro to ease type checking in stars.  badType is always
// OK.  TYPE_CHECK is assumed to be called in a member function of a star.
class NamedObj;
int badType(NamedObj& n,Packet& p,const char* typ);

#define TYPE_CHECK(pkt,type) if (badType(*this,pkt,type)) return

#endif

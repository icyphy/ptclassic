#ifndef _Message_h
#define  _Message_h 1

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

 This file defines the Envelope interface of Ptolemy.  A Envelope is
 an Message for passing objects of type Message around.  A
 MessageParticle is a type of Particle that transports Envelopes.

**************************************************************************/
#include "Particle.h"
#include "StringList.h"
#include "isa.h"

extern const DataType MESSAGE;

class Message {
	friend class Envelope;
public:
	// constructor.  Reference count is zero.
	Message() {
		INC_LOG_NEW; refCount = new int; *refCount = 0;
	}

	// copy constructor.  Copy always has a zero ref count.
	// arg is ignored since all objects are the same.  The
	// default copy constructor cannot be used because we must
	// have a separate refCount field.
	Message(const Message&) {
		INC_LOG_NEW; refCount = new int; *refCount = 0;
	}

	// destructor.
	virtual ~Message();

	// functions that may be specified by specific Envelopes.
	virtual int asInt() const;
	virtual double asFloat() const;
	virtual Complex asComplex() const;

	// type of the Message.  When overriding, be SURE
	// that the prototype matches (don't forget the const keywords)
	virtual const char* dataType() const;

	// specify how to print the Envelope.
	virtual StringList print() const;

	// clone the Envelope.  MUST BE REDEFINED by each derived class!
	// don't forget the "const" keyword when you do so!
	virtual Message* clone() const;

	// type checking: isA returns true if given the name of
	// the class or the name of any baseclass.  Exception:
	// the baseclass function returns FALSE to everything
	// (as it has no data at all).
	virtual int isA(const char*) const;
protected:
	int errorConvert(const char*) const;
private:
	// we use indirection for the reference count so it can be
	// manipulated even for a const Message object.
	int* refCount;
};

// A Envelope is simply a way of making a single Message look like
// multiple objects.

class Envelope {
public:
	// constructor: by default, point to dummyMessage.
	// dummyMessage is special, doesn't bother with ref counts.
	Envelope() : d(&dummyMessage) {}

	Envelope(Message& dat) : d(&dat) {
		incCount();
	}

	// copy constructor
	Envelope(const Envelope& p) {
		d = p.d;
		incCount();
	}
	// assignment operator
	Envelope& operator=(const Envelope& p) {
		p.incCount();
		unlinkData();
		d = p.d;
		return *this;
	}

	// destructor.  Wipe out the Message when the last
	// link is removed.
	~Envelope() {
		unlinkData();
	}

	// dataType() : pass through
	const char* dataType() const { return d->dataType();}

	// type check: checks Message type
	int typeCheck(const char* type) const {
		return d->isA(type);
	}

	// type error message generation
	// the pointer points to a static buffer!
	const char* typeError(const char* expected) const;

	// interfaces to Message functions
	int asInt() const { return d->asInt();}
	double asFloat() const { return d->asFloat();}
	Complex asComplex() const { return d->asComplex();}
	StringList print() const { return d->print();}

	// we can get a pointer to the Message.  It's a const
	// pointer so it cannot be used to alter the Message
	// (since there may be other references to it).
	const Message* myData() const { return d;}

	// produce a writable copy of the Message.  side effect --
	// contents of Envelope are changed to dummyMessage.
	Message* writableCopy();
private:
	static Message dummyMessage;

	// a pointer to my real data
	Message* d;

	// manipulate the reference count.  These MUST be used
	// properly.
	void incCount() const { (*d->refCount)++;}
	void decCount() const { (*d->refCount)--;}

	int refCount() const { return *d->refCount;}

	// bookkeeping function to zap the Message when done
	void unlinkData();
};

// A Particle class to transmit Messages (which are enclosed in Envelopes)
class MessageParticle : public Particle {
public:
	DataType type() const;

	operator int () const;
	operator float () const;
	operator double () const;
	operator Complex () const;
	StringList print() const;

	void getMessage (Envelope& p);
	void accessMessage (Envelope& p) const;

	// fill in remaining functions for Particle classes

	MessageParticle(const Envelope& p) : data(p) {}
	MessageParticle() {}
	void initialize();

	// load with data -- these cause errors except for a Envelope argument.

	void operator << (int i);
	void operator << (double f);
	void operator << (const Complex& c);
	void operator << (const Envelope& p);

	// particle copy
	Particle& operator = (const Particle& p);

	// compare particles
	int operator == (const Particle&);

	// clone, useNew, die analogous to other particles.

	Particle* clone();

	Particle* useNew();
	void die();

private:
	void errorAssign(const char*) const;
	Envelope data;
};

// function and macro to ease type checking in stars.  badType is always
// OK.  TYPE_CHECK is assumed to be called in a member function of a star.
class NamedObj;
int badType(NamedObj& n,Envelope& p,const char* typ);

#define TYPE_CHECK(pkt,type) if (badType(*this,pkt,type)) return

#endif

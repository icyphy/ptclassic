#ifndef _Message_h
#define  _Message_h 1

#ifdef __GNUG__
#pragma interface
#endif
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer:  J. T. Buck
 Date of creation: 2/20/91

 This file defines the Envelope interface of Ptolemy.  A Envelope is
 an Message for passing objects of type Message around.  A
 MessageParticle is a type of Particle that transports Envelopes.

**************************************************************************/
#include "Particle.h"
#include "StringList.h"
#include "Fix.h"
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
	virtual Fix asFix() const;

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
private:
	// fns to manipulate the reference count.  These MUST be used
	// properly.  They are first so they can be included
	// by other inline functions and still be inlined correctly.
	void incCount() const { (*d->refCount)++;}

	// decCount returns the new count as its value.
	int decCount() const { return --(*d->refCount);}

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

	// assignment operator.  Adjusts reference counts; possibly
	// deletes the old Message I pointed to.
	Envelope& operator=(const Envelope& p);

	// destructor.  Wipes out the Message when the last
	// link is removed.
	~Envelope();

	// an envelope is "empty" if it has the dummy message.
	int empty() const { return (d == &dummyMessage);}

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
	Fix asFix() const { return d->asFix();}
	StringList print() const { return d->print();}

	// we can get a pointer to the Message.  It's a const
	// pointer so it cannot be used to alter the Message
	// (since there may be other references to it).
	const Message* myData() const { return d;}

	// produce a writable copy of the Message.  side effect --
	// contents of Envelope are changed to dummyMessage.
	Message* writableCopy();
protected:
	// return the reference count of the Message.
	int refCount() const { return *d->refCount;}
private:
	// bookkeeping function to zap the Message when done
	void unlinkData() {
		if (!empty() && decCount() == 0) {
			INC_LOG_DEL; delete d;
		}
	}

	// the special "null message".
	static Message dummyMessage;

	// a pointer to my real data
	Message* d;

};

// A Particle class to transmit Messages (which are enclosed in Envelopes)
class MessageParticle : public Particle {
public:
	DataType type() const;

	operator int () const;
	operator float () const;
	operator double () const;
	operator Complex () const;
	operator Fix () const;
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
	void operator << (const Fix& c);
	void operator << (const Envelope& p);

	// particle copy
	Particle& operator = (const Particle& p);

	// compare particles
	int operator == (const Particle&);

	// clone, useNew, die analogous to other particles.

	Particle* clone() const;

	Particle* useNew() const;
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

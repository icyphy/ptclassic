static const char file_id[] = "Message.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 2/20/91

 This file defines the Envelope implementation of Ptolemy.  A Envelope is
 an envelope for passing objects derived from type EnvelopeData around.
 A  MessageParticle is a type of Particle that transports Envelopes.

***************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Message.h"
#include "Particle.h"
#include "Plasma.h"
#include "Error.h"

Message Envelope::dummyMessage;

// methods for Message.  These are typically overridden; they are
// all virtual.

// destructor for Message: zap storage for reference count.
Message::~Message() {
	LOG_DEL; delete refCount;
}

// methods for conversion to other types
int Message::asInt() const {
	return errorConvert("int");
}
double Message::asFloat() const {
	return errorConvert("float");
}
Complex Message::asComplex() const {
	return errorConvert("complex");
}

const char* Message::dataType() const { return "DUMMY";}

int Message::isA(const char*) const {
	return FALSE;
}

StringList Message::print() const {
	StringList out = "<";
	out += dataType();
	out += ">: no print method";
	return out;
}

int Message::errorConvert(const char* arg) const {
	StringList msg = "Envelope type '";
	msg += dataType();
	msg += "': invalid conversion to ";
	msg += arg;
	Error::abortRun(msg);
	return 0;
}

// clone -- try to catch some errors for folks who don't redefine it.
Message* Message::clone() const {
	LOG_NEW; Message* p = new Message;
	if (strcmp(dataType(),p->dataType()) != 0)
		Error::abortRun("Message class ",dataType(),
				" doesn't redefine clone()!");
	return p;
}

// The following is a permanent Envelope with contents "dummyMessage".
static Envelope dummy;

extern const DataType MESSAGE = "MESSAGE";

// assignment operator.  Adjust reference counts.

Envelope& Envelope::operator=(const Envelope& p) {
	p.incCount();
	unlinkData();
	d = p.d;
	return *this;
}

// destructor.  Wipe out the Message when the last
// link is removed.

Envelope::~Envelope() {
	unlinkData();
}

// Envelope error message generation.  The message is in a static buffer.
const char* Envelope::typeError(const char* expected) const {
	static char buf[80];
	sprintf (buf, "Expected Envelope type '%s', got '%s'",expected,dataType());
	return buf;
}

// generate a writable copy of the contents of a Envelope.
// The writable copy always has reference count 0, and the Envelope
// itself is modified to point to dummyMessage.
Message* Envelope::writableCopy() {
	Message* result;
	decCount();
	if (refCount() > 0) {
		result = d->clone();
		*(result->refCount) = 0;
	}
	else result = d;
	d = &dummyMessage;
	return result;
}

// *********************************************************** //

static MessageParticle pproto;
static Plasma messagePlasma(pproto);

// MessageParticle methods

DataType MessageParticle::type() const { return MESSAGE;}

MessageParticle::operator int () const {
	return data.asInt();
}

MessageParticle::operator double () const {
	return data.asFloat();
}

MessageParticle::operator float () const {
	return float(data.asFloat());
}

MessageParticle::operator Complex () const {
	return data.asComplex();
}

StringList MessageParticle::print() const {
	return data.print();
}

// get the Message from the MessageParticle.
void MessageParticle::accessMessage (Envelope& p) const {
	p = data;
}

// get the Message and remove the reference (by setting data to dummy)
void MessageParticle::getMessage (Envelope& p) {
	p = data;
	data = dummy;
}

void MessageParticle::initialize() { data = dummy;}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void MessageParticle::operator << (int) { errorAssign("int");}
void MessageParticle::operator << (double) { errorAssign("double");}
void MessageParticle::operator << (const Complex&) { errorAssign("complex");}

// only loader that works.
void MessageParticle::operator << (const Envelope& p) { data = p;}

// particle copy
Particle& MessageParticle::operator = (const Particle& p) {
	if (compareType(p)) {
		const MessageParticle& ps = *(const MessageParticle*)&p;
		data = ps.data;
	}
	return *this;
}

// particle compare: considered equal if Message addresses
// are the same.
int MessageParticle :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	Envelope pkt;
	p.accessMessage(pkt);
	return data.myData() == pkt.myData();
}

// clone, useNew, die analogous to other particles.

Particle* MessageParticle::clone() const {
	Particle * p = messagePlasma.get();
	((MessageParticle*)p)->data = data;
	return p;
}

Particle* MessageParticle::useNew() const {
	LOG_NEW; return new MessageParticle;
}

// We assign "dummy" to the Envelope before returning it to the Plasma.
// this removes references to other Message objects and allows them
// to be deleted when no longer required.
void MessageParticle::die() {
	data = dummy;
	messagePlasma.put(this);
}

void MessageParticle::errorAssign(const char* argType) const {
	StringList msg = "Attempt to assign type ";
	msg += argType;
	msg += " to a MessageParticle";
	Error::abortRun(msg);
}

// an error checker:
int badType(NamedObj& where,Envelope& pkt,const char* type) {
	if (!pkt.typeCheck(type)) {
		Error::abortRun(where, pkt.typeError(type));
		return TRUE;
	}
	return FALSE;
}

/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 2/20/91

 This file defines the Packet implementation of Ptolemy.  A Packet is
 an envelope for passing objects of type PacketData around.  A
 PacketSample is a type of Particle that transports Packets.

***************************************************************/
#include "Packet.h"
#include "Particle.h"
#include "Plasma.h"
#include "Error.h"

PacketData Packet::dummyPacket;

// methods for PacketData.  These are typically overridden; they are
// all virtual.

// dummy destructor
PacketData::~PacketData() {}

// methods for conversion to other types
int PacketData::asInt() const {
	return errorConvert("int");
}
float PacketData::asFloat() const {
	return errorConvert("float");
}
Complex PacketData::asComplex() const {
	return errorConvert("complex");
}

const char* PacketData::dataType() const { return "DUMMY";}

StringList PacketData::print() const {
	StringList out = "<";
	out += dataType();
	out += ">: no print method";
}

int PacketData::errorConvert(const char* arg) const {
	StringList msg = "Packet type '";
	msg += dataType();
	msg += "': invalid conversion to ";
	msg += arg;
	Error::abortRun(msg);
	return 0;
}

// having the following prevents removal of last reference to
// dummyPacket.
static Packet dummy;

extern const dataType PACKET = "PACKET";

// Packet error message generation
StringList Packet::typeError(const char* expected) const {
	StringList msg = " Expected packet type '";
	msg += expected;
	msg += "', got '";
	msg += dataType();
	msg += "'";
}

static PacketSample pproto;
static Plasma packetPlasma(pproto);

// PacketSample methods

dataType PacketSample::readType() const { return PACKET;}

PacketSample::operator int () const {
	return data.asInt();
}

PacketSample::operator float () const {
	return data.asFloat();
}

PacketSample::operator Complex () const {
	return data.asComplex();
}

StringList PacketSample::print() const {
	return data.print();
}

void PacketSample::getPacket (Packet& p) const { p = data;}

void PacketSample::initialize() { data = dummy;}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void PacketSample::operator << (int i) { errorAssign("int");}
void PacketSample::operator << (float f) { errorAssign("float");}
void PacketSample::operator << (Complex& c) { errorAssign("complex");}

// only loader that works.
void PacketSample::operator << (const Packet& p) { data = p;}

// particle copy
Particle& PacketSample::operator = (const Particle& p) {
	if (compareType(p))
		data = ((PacketSample&)p).data;
	return *this;
}

// clone, useNew, die analogous to other particles.

Particle* PacketSample::clone() {
	Particle * p = packetPlasma.get();
	((PacketSample*)p)->data = data;
	return p;
}

Particle* PacketSample::useNew() {
	return new PacketSample;
}

// We assign "dummy" to the packet before returning it to the Plasma.
// this removes references to other PacketData objects and allows them
// to be deleted when no longer required.
void PacketSample::die() {
	data = dummy;
	packetPlasma.put(this);
}

void PacketSample::errorAssign(const char* argType) const {
	StringList msg = "Attempt to assign type ";
	msg += argType;
	msg += " to a Packet Particle";
	Error::abortRun(msg);
}

// Error catcher for attempts to retrieve a Packet from a different
// type of particle

void Particle::getPacket(Packet &) const {
	Error::abortRun ("Attempt to getPacket from a non-packet Particle");
}

void Particle::operator<<(const Packet&) {
	Error::abortRun ("Attempt to load a Packet into non-packet Particle");
}

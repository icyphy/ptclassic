static const char file_id[] = "Packet.cc";
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 2/20/91

 This file defines the Packet implementation of Ptolemy.  A Packet is
 an envelope for passing objects derived from type PacketData around.
 A  PacketSample is a type of Particle that transports Packets.

***************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "Packet.h"
#include "Particle.h"
#include "Plasma.h"
#include "Error.h"

PacketData Packet::dummyPacket;

// methods for PacketData.  These are typically overridden; they are
// all virtual.

// destructor for PacketData: zap storage for reference count.
PacketData::~PacketData() {
	LOG_DEL; delete refCount;
}

// methods for conversion to other types
int PacketData::asInt() const {
	return errorConvert("int");
}
double PacketData::asFloat() const {
	return errorConvert("float");
}
Complex PacketData::asComplex() const {
	return errorConvert("complex");
}

const char* PacketData::dataType() const { return "DUMMY";}

int PacketData::isA(const char*) const {
	return FALSE;
}

StringList PacketData::print() const {
	StringList out = "<";
	out += dataType();
	out += ">: no print method";
	return out;
}

int PacketData::errorConvert(const char* arg) const {
	StringList msg = "Packet type '";
	msg += dataType();
	msg += "': invalid conversion to ";
	msg += arg;
	Error::abortRun(msg);
	return 0;
}

// clone -- try to catch some errors for folks who don't redefine it.
PacketData* PacketData::clone() const {
	LOG_NEW; PacketData* p = new PacketData;
	if (strcmp(dataType(),p->dataType()) != 0)
		Error::abortRun("PacketData class ",dataType(),
				"doesn't redefine clone()!");
	return p;
}

// The following is a permanent packet with contents "dummyPacket".
static Packet dummy;

// bookkeeping function to zap the PacketData when done
// have to handle dummyPacket specially (it cannot be deleted)
void Packet::unlinkData() {
	if (d == &dummyPacket) return;
	decCount();
	if (refCount() == 0) {
		LOG_DEL; delete d;
	}
}

extern const DataType PACKET = "PACKET";

// Packet error message generation.  The message is in a static buffer.
const char* Packet::typeError(const char* expected) const {
	static char buf[80];
	sprintf (buf, "Expected packet type '%s', got '%s'",expected,dataType());
	return buf;
}

// generate a writable copy of the contents of a packet.
// The writable copy always has reference count 0, and the packet
// itself is modified to point to dummyPacket.
PacketData* Packet::writableCopy() {
	PacketData* result;
	decCount();
	if (refCount() > 0) {
		result = d->clone();
		*(result->refCount) = 0;
	} else {
		result = d;
		d = &dummyPacket;
	}
	return result;
}

// *********************************************************** //

static PacketSample pproto;
static Plasma packetPlasma(pproto);

// PacketSample methods

DataType PacketSample::readType() const { return PACKET;}

PacketSample::operator int () const {
	return data.asInt();
}

PacketSample::operator double () const {
	return data.asFloat();
}

PacketSample::operator float () const {
	return float(data.asFloat());
}

PacketSample::operator Complex () const {
	return data.asComplex();
}

StringList PacketSample::print() const {
	return data.print();
}

// get the PacketData from the PacketSample.
void PacketSample::accessPacket (Packet& p) const {
	p = data;
}

// get the PacketData and remove the reference (by setting data to dummy)
void PacketSample::getPacket (Packet& p) {
	p = data;
	data = dummy;
}

void PacketSample::initialize() { data = dummy;}

// load with data -- function errorAssign prints an
// error and calls Error::abortRun().

void PacketSample::operator << (int) { errorAssign("int");}
void PacketSample::operator << (double) { errorAssign("double");}
void PacketSample::operator << (const Complex&) { errorAssign("complex");}

// only loader that works.
void PacketSample::operator << (const Packet& p) { data = p;}

// particle copy
Particle& PacketSample::operator = (const Particle& p) {
	if (compareType(p)) {
		const PacketSample& ps = *(const PacketSample*)&p;
		data = ps.data;
	}
	return *this;
}

// particle compare: considered equal if packetdata addresses
// are the same.
int PacketSample :: operator == (const Particle& p) {
	if (!typesEqual(p)) return 0;
	Packet pkt;
	p.accessPacket(pkt);
	return data.myData() == pkt.myData();
}

// clone, useNew, die analogous to other particles.

Particle* PacketSample::clone() {
	Particle * p = packetPlasma.get();
	((PacketSample*)p)->data = data;
	return p;
}

Particle* PacketSample::useNew() {
	LOG_NEW; return new PacketSample;
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

// an error checker:
int badType(NamedObj& where,Packet& pkt,const char* type) {
	if (!pkt.typeCheck(type)) {
		Error::abortRun(where, pkt.typeError(type));
		return TRUE;
	}
	return FALSE;
}

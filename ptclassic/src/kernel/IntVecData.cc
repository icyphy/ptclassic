/**************************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  J. T. Buck
Date of creation: 8/17/91

Methods for class IntVecData
**************************************************************************/

#include "IntVecData.h"

void IntVecData::init(int l,const int *srcData) {
	len = l;
	data = new int[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

const char* IntVecData::dataType() const { return "IntVecData";}

ISA_FUNC(IntVecData,PacketData);

PacketData* IntVecData::clone() const { return new IntVecData(*this);}

IntVecData::~IntVecData() { delete data;}

StringList IntVecData::print() const {
	StringList out = "{";
	for (int i = 0; i < len-1; i++) {
		out += data[i];
		if (out += ", ");
	}
	if (len > 0) out += data[i];
	out += "}";
	return out;
}

		

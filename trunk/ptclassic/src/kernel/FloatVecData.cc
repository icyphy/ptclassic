/**************************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  J. T. Buck
Date of creation: 8/17/91

Methods for class FloatVecData
**************************************************************************/

#include "FloatVecData.h"

void FloatVecData::init(int l,const float *srcData) {
	len = l;
	data = new float[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

const char* FloatVecData::dataType() const { return "FloatVecData";}

ISA_FUNC(FloatVecData,PacketData);

PacketData* FloatVecData::clone() const { return new FloatVecData(*this);}

FloatVecData::~FloatVecData() { delete data;}

StringList FloatVecData::print() const {
	StringList out = "{";
	for (int i = 0; i < len-1; i++) {
		out += data[i];
		if (out += ", ");
	}
	if (len > 0) out += data[i];
	out += "}";
	return out;
}

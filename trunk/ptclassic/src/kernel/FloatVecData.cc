static const char file_id[] = "FloatVecData.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  J. T. Buck
Date of creation: 8/17/91

Methods for class FloatVecData
**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include "FloatVecData.h"

void FloatVecData::init(int l,const float *srcData) {
	len = l;
	LOG_NEW; data = new float[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

const char* FloatVecData::dataType() const { return "FloatVecData";}

ISA_FUNC(FloatVecData,PacketData);

PacketData* FloatVecData::clone() const { LOG_NEW; return new FloatVecData(*this);}

FloatVecData::~FloatVecData() { LOG_DEL; delete data;}

StringList FloatVecData::print() const {
	StringList out = "{";
	for (int i = 0; i < len-1; i++) {
		out += data[i];
		out += ", ";
	}
	if (len > 0) out += data[i];
	out += "}";
	return out;
}

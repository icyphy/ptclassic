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

// constructor: makes an initialized array from a double array
FloatVecData::FloatVecData(int l,const double *srcData) : len(l) {
	LOG_NEW; data = new float[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

	// assignment operator
const FloatVecData& FloatVecData::operator=(const FloatVecData& src) {
	if (data != src.data) {
		LOG_DEL; delete [] data;
		init(src.len,src.data);
	}
	return *this;
}

const char* FloatVecData::dataType() const { return "FloatVecData";}

ISA_FUNC(FloatVecData,Message);

Message* FloatVecData::clone() const { LOG_NEW; return new FloatVecData(*this);}

FloatVecData::~FloatVecData() { LOG_DEL; delete [] data;}

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

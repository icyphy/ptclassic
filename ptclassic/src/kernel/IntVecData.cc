static const char file_id[] = "IntVecData.cc";
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  J. T. Buck
Date of creation: 8/17/91

Methods for class IntVecData
**************************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif
#include "IntVecData.h"

void IntVecData::init(int l,const int *srcData) {
	len = l;
	LOG_NEW; data = new int[l];
	for (int i = 0; i < l; i++)
		data[i] = *srcData++;
}

const char* IntVecData::dataType() const { return "IntVecData";}

ISA_FUNC(IntVecData,Message);

Message* IntVecData::clone() const { LOG_NEW; return new IntVecData(*this);}

IntVecData::~IntVecData() { LOG_DEL; delete data;}

StringList IntVecData::print() const {
	StringList out = "{";
	for (int i = 0; i < len-1; i++) {
		out += data[i];
		out += ", ";
	}
	if (len > 0) out += data[i];
	out += "}";
	return out;
}

		

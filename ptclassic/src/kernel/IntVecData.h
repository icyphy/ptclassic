#ifndef _IntVecData_h
#define _IntVecData_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  J. T. Buck
Date of creation: 4/3/91

This is a simple vector packet body.  It stores an array of integer values
of arbitrary length.  The length is specified by the constructor.
**************************************************************************/

#include "Packet.h"

class IntVecData : public PacketData {
private:
	int len;

	init(int l,int *srcData) {
		len = l;
		data = new int[l];
		for (int i = 0; i < l; i++)
			data[i] = *srcData++;
	}

public:
	// the pointer is public for simplicity
	int *data;

	int length() const { return len;}
	const char* dataType() const { return "IntVecData";}

	// constructor: makes an uninitialized array
	IntVecData(int l) : len(l) {
		data = new int[l];
	}

	// constructor: makes an initialized array from a int array
	IntVecData(int l,int *srcData) { init(l,srcData);}

	// copy constructor
	IntVecData(const IntVecData& src) { init(src.len,src.data);}

	// clone
	PacketData* clone() const { return new IntVecData(*this);}

	// destructor
	~IntVecData() {
		delete data;
	}
};
#endif

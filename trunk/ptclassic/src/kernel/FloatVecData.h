#ifndef _FloatVecData_h
#define _FloatVecData_h 1
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  J. T. Buck
Date of creation: 4/3/91

This is a simple vector packet body.  It stores an array of float values
of arbitrary length.  The length is specified by the constructor.
**************************************************************************/

#include "Packet.h"

class FloatVecData : public PacketData {
private:
	int len;

	void init(int l,const float *srcData);

public:
	// the pointer is public for simplicity
	float *data;

	int length() const { return len;}
	const char* dataType() const;
	int isA(const char*) const;

	// constructor: makes an uninitialized array
	FloatVecData(int l) : len(l) {
		data = new float[l];
	}

	// constructor: makes an initialized array from a float array
	FloatVecData(int l,const float *srcData) { init(l,srcData); }

	// copy constructor
	FloatVecData(const FloatVecData& src) { init(src.len,src.data);}

	// constructor: makes an initialized array from a double array
	FloatVecData(int l,const double *srcData) : len(l) {
		data = new float[l];
		for (int i = 0; i < l; i++)
			data[i] = *srcData++;
	}

	// clone
	PacketData* clone() const;

	// print
	StringList print() const;

	// destructor
	~FloatVecData();
};
#endif

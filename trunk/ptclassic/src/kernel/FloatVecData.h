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
#ifdef __GNUG__
#pragma interface
#endif

#include "Message.h"

class FloatVecData : public Message {
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
		INC_LOG_NEW; data = new float[l];
	}

	// constructor: makes an initialized array from a float array
	FloatVecData(int l,const float *srcData) { init(l,srcData); }

	// copy constructor
	FloatVecData(const FloatVecData& src) { init(src.len,src.data);}

	// assignment operator
	const FloatVecData& operator=(const FloatVecData& src);

	// constructor: makes an initialized array from a double array
	FloatVecData(int l,const double *srcData);

	// clone
	Message* clone() const;

	// print
	StringList print() const;

	// destructor
	~FloatVecData();
};
#endif

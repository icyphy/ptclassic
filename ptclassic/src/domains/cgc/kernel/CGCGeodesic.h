#ifndef _CGCGeodesic_h
#define _CGCGeodesic_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 Geodesic for C code generation.
 In addition to fork management provide by CG Geodesics,
 they manage definition and naming of buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"

class CGCGeodesic : public CGGeodesic {
public:
	CGCGeodesic() : bufName(0), myWaste(2.0) {}
	~CGCGeodesic() {LOG_DEL; delete bufName; }

	// class identification
	int isA(const char*) const;

	void setBufName(char* n) {bufName = n;}
	char* getBufName() const;

	// make large the wasteFactor
	void preferLinearBuf(int i) { if (i) myWaste = 1e9; }

protected:
	// Redefine: decide the wasteFactor for linear buffering.
	// Look at the comment in CGGeodesic class.
	double wasteFactor() const;

private:
	// Name of the C object used as a buffer
	char* bufName;
	// wasteFactor
	double myWaste;
};

#endif

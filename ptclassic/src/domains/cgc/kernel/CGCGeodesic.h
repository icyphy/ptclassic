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
	CGCGeodesic() : bufName(0) {}
	~CGCGeodesic() {LOG_DEL; delete bufName; }

	// class identification
	int isA(const char*) const;

	void setBufName(char* n) {bufName = n;}
	char* getBufName() const;

private:
	// Name of the C object used as a buffer
	char* bufName;
};

#endif

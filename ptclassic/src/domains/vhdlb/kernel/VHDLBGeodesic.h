#ifndef _VHDLBGeodesic_h
#define _VHDLBGeodesic_h 1
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: Edward A. Lee

 Geodesic for VHDLB code generation.
 In addition to fork management provide by VHDLBGeodesics,
 they manage definition and naming of buffers.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"

class VHDLBGeodesic : public CGGeodesic {
public:
	VHDLBGeodesic() : bufName(0) {}
	~VHDLBGeodesic() {LOG_DEL; delete bufName; }

	// class identification
	int isA(const char*) const;

	void setBufName(char* n) {bufName = n;}
	char* getBufName() const;

private:
	// Name of the VHDLB variable used as a buffer
	char* bufName;
};
#endif

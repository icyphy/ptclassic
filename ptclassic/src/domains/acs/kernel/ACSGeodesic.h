// @(#)ACSGeodesic.h	1.3 03/18/98
#ifndef _ACSGeodesic_h
#define _ACSGeodesic_h

#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"
class ACSPortHole;

class ACSGeodesic : public CGGeodesic {
public:
	ACSGeodesic() : bufName(0), myWaste(2.0) {}
	~ACSGeodesic() {LOG_DEL; delete [] bufName; }

	// class identification
	int isA(const char*) const;

	void setBufName(const char* n) {
		delete [] bufName; bufName = savestring(n);
	}
	char* getBufName() const;

	// make large the wasteFactor
	void preferLinearBuf(int i) { if (i) myWaste = 1e9; }

	// Return the size of the "local buffer" connected to this
	// PortHole.  This returns zero for cases where no separate
	// buffer is allocated, e.g. fork outputs (all destinations
	// of the fork share the same buffer, whose size is returned
	// by bufSize).
	// virtual int localBufSize() const;

protected:
//	ACSPortHole* src();
//	const ACSPortHole* src() const;
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

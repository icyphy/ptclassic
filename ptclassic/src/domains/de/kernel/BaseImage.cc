// filename:		BaseImage.cc
// author:			Paul Haskell
// creation date:	7/1/91
// SCCS info:		$Id$


#include "BaseImage.h"

/********* strstr is part of the regular lib, but not of Ptolemy
This will be replaced by the "isA()" function in PacketData class,
when that function becomes supported.
extern "C" char* strstr(const char*, const char*);
*********/
const char* StrStr(const char* a, const char* b)
{
	const char* origB = b;
	const char* retval = (char*) NULL;
	while ((*a != '\000') && (*b != '\000')) {
		while ((*a != '\000') && (*a != *b)) { a++; }
		retval = a;
		while ((*a!='\000') && (*b!='\000') && (*a==*b)) {
			a++; b++;
		}
		if (*b == '\000') { return retval; }
		else { b = origB; retval = (char*) NULL; }
	}
	return(retval);
}


void BaseImage::copy(int len, float* into, float* from)
{
	for(int travel = 0; travel < len%5; travel++) {
		into[travel] = from[travel];
	}
	for(travel = len%5; travel < len; travel+=5) {
		into[travel] = from[travel];
		into[travel+1] = from[travel+1];
		into[travel+2] = from[travel+2];
		into[travel+3] = from[travel+3];
		into[travel+4] = from[travel+4];
	}
}


void BaseImage::copy(int len, unsigned char* into,
		unsigned char* from)
{
	for(int travel = 0; travel < len%5; travel++) {
		into[travel] = from[travel];
	}
	for(travel = len%5; travel < len; travel+=5) {
		into[travel] = from[travel];
		into[travel+1] = from[travel+1];
		into[travel+2] = from[travel+2];
		into[travel+3] = from[travel+3];
		into[travel+4] = from[travel+4];
	}
} // end BaseImage::copy()

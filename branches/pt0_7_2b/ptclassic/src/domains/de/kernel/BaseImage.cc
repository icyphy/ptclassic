// filename:       BaseImage.cc
// author:         Paul Haskell
// creation date:  7/1/91
// SCCS info:      $Id$

#include "BaseImage.h"

BaseImage::BaseImage(int a, int b, int c, int d):
            width(a), height(b), frameId(c), fullSize(d)
            { startPos = 0; size = fullSize; }

BaseImage::BaseImage(int a, int b, int c):
            width(a), height(b), frameId(c)
            { startPos = 0; size = fullSize = width*height; }

BaseImage::BaseImage(BaseImage& bi):
            width(bi.width), height(bi.height), frameId(bi.frameId),
            startPos(bi.startPos), size(bi.size), fullSize(bi.fullSize)
            { }

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
} // end BaseImage::copy()


void BaseImage::copy(int len, char* into, char* from)
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

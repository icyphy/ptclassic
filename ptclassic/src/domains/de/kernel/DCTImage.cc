// filename:		DCTImage.cc
// author:			Sun-Inn Shih
// creation date:	7/11/91
// SCCS info:		$Id$

#include <math.h>
#include "DCTImage.h"


void DCTImage::init()
{
// Round width and height up to be multiples of "blocksize".
	upWidth = width; upHeight = height;
	if ( (upWidth%blocksize) != 0) {
		upWidth = blocksize * (upWidth/blocksize + 1);
	}
	if ( (upHeight%blocksize) != 0) {
		upHeight = blocksize * (upHeight/blocksize + 1);
	}
	size = fullSize = upWidth*upHeight;

	DCTData = new float[fullSize];
} // end DCTImage::init()


DCTImage::DCTImage(int a, int b, int c, int d, int e):
		blocksize(e), BaseImage(a, b, c, d) { init(); }


DCTImage::DCTImage(int a, int b, int c, int d):
		blocksize(d), BaseImage(a, b, c) { init(); }


DCTImage::DCTImage(const BaseImage& bi, int a):
		blocksize(a), BaseImage(bi) { init(); }


DCTImage::DCTImage(const DCTImage& di, int a):
		blocksize(di.blocksize), upWidth(di.upWidth),
		upHeight(di.upHeight), BaseImage(di)
{
	DCTData = new float[size];
	if (!a) { copy(size, DCTData, di.DCTData); }
} // end DCTImage::DCTImage()


DCTImage::~DCTImage()
{ delete DCTData; DCTData = (float*) NULL; }


BaseImage* DCTImage::fragment(int cellSz, int Num)
{
	int arrSz = cellSz/4; // 4 == sizeof(float)

	if (Num*arrSz > size) { return((BaseImage*) NULL); }

// Use clone(1) rather than clone() to avoid copying image data.
	DCTImage* retval = (DCTImage*) clone(1);
	retval->startPos = startPos + Num*arrSz;
	retval->size = min(startPos+size-retval->startPos, arrSz);
	delete retval->DCTData;
	retval->DCTData = new float[retval->size];

	int offset = retval->startPos - startPos;
	copy(retval->size, retval->DCTData, DCTData+offset);
	return(retval);
} // end DCTImage::fragment()


void DCTImage::assemble(const BaseImage* bi)
{
// Do we have an acceptable image to merge?
	if ( !bi->isA("DCTImage") || (*bi != *this) ) return;

// Are we set up to merge yet?
	if (size != fullSize) {
		float* tmpPtr = new float[fullSize];
		copy(size, tmpPtr+startPos, DCTData);
		delete DCTData;
		DCTData = tmpPtr;
		size = fullSize;
	}

// Do the merge
	DCTImage* di = (DCTImage*) bi;
	copy(di->size, DCTData+di->startPos, di->DCTData);
} // end DCTData::assemble()

const char* DCTImage::dataType() const { return("DCTImage"); }
PacketData* DCTImage::clone() const { return new DCTImage(*this); }
PacketData* DCTImage::clone(int a) const { return new DCTImage(*this, a); }
ISA_FUNC(DCTImage,BaseImage);

static const char file_id[] = "DCTImage.cc";
// filename:		DCTImage.cc
// author:			Sun-Inn Shih
// creation date:	7/11/91
// SCCS info:		$Id$

#include "DCTImage.h"
#include <minmax.h>

const char* DCTImage::dataType() const { return("DCTImage"); }
Message* DCTImage::clone() const { LOG_NEW; return new DCTImage(*this); }
Message* DCTImage::clone(int a) const {LOG_NEW; return new DCTImage(*this,a);}
ISA_FUNC(DCTImage,BaseImage);


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

	LOG_NEW; DCTData = new float[fullSize];
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
	LOG_NEW; DCTData = new float[size];
	if (!a) { copy(size, DCTData, di.DCTData); }
} // end DCTImage::DCTImage()


DCTImage::~DCTImage()
{ LOG_DEL; delete [] DCTData; DCTData = (float*) NULL; }


void DCTImage::setSize(const int a)
{
	if (size != fullSize) return;
	if (a == fullSize) return;
	LOG_DEL; delete [] DCTData;
	LOG_NEW; DCTData = new float[a];
	size = fullSize = a;
} // end DCTImage::setSize()


float* DCTImage::retData()
{ return DCTData; }


BaseImage* DCTImage::fragment(int cellSz, int Num) const
{
	int arrSz = cellSz/4; // 4 == sizeof(float)

	if (Num*arrSz > size) { return((BaseImage*) NULL); }

// Use clone(1) rather than clone() to avoid copying image data.
	DCTImage* retval = (DCTImage*) clone(1);
	retval->startPos = startPos + Num*arrSz;
	retval->size = min(startPos+size-retval->startPos, arrSz);
	LOG_DEL; delete [] retval->DCTData;
	LOG_NEW; retval->DCTData = new float[retval->size];

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
		LOG_NEW; float* tmpPtr = new float[fullSize];
		for(int t = 0; t < fullSize; t++) { tmpPtr[t] = 0.0; }
		copy(size, tmpPtr+startPos, DCTData);
		LOG_DEL; delete [] DCTData;
		DCTData = tmpPtr;
		size = fullSize;
	}

// Do the merge
	const DCTImage* di = (const DCTImage*) bi;
	copy(di->size, DCTData+di->startPos, di->DCTData);
} // end DCTData::assemble()

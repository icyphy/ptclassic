static const char file_id[] = "GrayImage.cc";
// filename:		GrayImage.cc
// author:			Paul Haskell
// creation date:	7/1/91
// SCCS info:		$Id$

#include "GrayImage.h"
#include <minmax.h>

const char* GrayImage::dataType() const { return "GrayImage";}
PacketData* GrayImage::clone() const { LOG_NEW; return new GrayImage(*this);}
PacketData* GrayImage::clone(int a) const { LOG_NEW; return new GrayImage(*this,a);}
ISA_FUNC(GrayImage,BaseImage);

void GrayImage::init()
{ LOG_NEW; grayData = new unsigned char[fullSize]; }


GrayImage::GrayImage(int a, int b, int c, int d):
		BaseImage(a, b, c, d) { init(); }


GrayImage::GrayImage(int a, int b, int c):
		BaseImage(a, b, c) { init(); }


GrayImage::GrayImage(const BaseImage& bi):
		BaseImage(bi) { init(); }


GrayImage::GrayImage(const GrayImage& gi, int a):
		BaseImage(gi)
{
	init();
	if (!a) { copy(size, grayData, gi.grayData); }
} // end GrayImage::GrayImage()


GrayImage::~GrayImage()
{ LOG_DEL; delete [] grayData; grayData = (unsigned char*) NULL; }


void GrayImage::setSize(int a)
{
	if (size != fullSize) return;
	if (a == fullSize) return;
	LOG_DEL; delete [] grayData;
	LOG_NEW; grayData = new unsigned char[a];
	size = fullSize = a;
} // GrayImage::setSize()


unsigned char* GrayImage::retData()
{ return grayData; }


BaseImage* GrayImage::fragment(int cellSz, int Num) const
{
	if (Num*cellSz > size) return ((BaseImage*) NULL);

// We use clone(int) rather than clone() in order to avoid copying
// image data needlessly.
	GrayImage* retval = (GrayImage*) clone(1);
	retval->startPos = startPos + Num*cellSz;
	retval->size = min(startPos+size-retval->startPos, cellSz);
	LOG_DEL; delete [] retval->grayData; // some other way to do this?
	LOG_NEW; retval->grayData = new unsigned char[retval->size];

	int offset = retval->startPos - startPos;
	copy(retval->size, retval->grayData, grayData+offset);
	return(retval);
} // end GrayImage::fragment()


void GrayImage::assemble(const BaseImage* bi)
{
// Do we have an acceptable image to merge?
	if (!bi->isA("GrayImage") || (*bi != *this) ) return;

// Are we set up to merge yet?
	if (size != fullSize) {
		LOG_NEW; unsigned char* tmpPtr = new unsigned char[fullSize];
		for(int t = 0; t < fullSize; t++) {tmpPtr[t]=(unsigned char) 0;}
		copy(size, tmpPtr+startPos, grayData);
		LOG_DEL; delete [] grayData;
		grayData = tmpPtr;
		size = fullSize;
	}

// Do the merge
	const GrayImage* gi = (const GrayImage*) bi;
	copy(gi->size, grayData + gi->startPos, gi->grayData);
} // end GrayData::assemble()

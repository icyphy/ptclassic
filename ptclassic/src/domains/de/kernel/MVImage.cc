// filename:		MVImage.cc
// author:			Paul Haskell
// creation date:	7/10/91
// SCCS info:		$Id$

#include "MVImage.h"

void MVImage::init()
{
	if (blocksize <= 0) blocksize = 8;
	horzData = new char[fullSize];
	vertData = new char[fullSize];
} // end MVImage::init()


MVImage::MVImage(): blocksize(0), BaseImage(0, 0, 0)
{ horzData = vertData = (char*) NULL; }


MVImage::MVImage(int a, int b, int c, int d, int e):
		blocksize(e), BaseImage(a, b, c, d) { init(); }


MVImage::MVImage(int a, int b, int c, int d):
		blocksize(d), BaseImage(a, b, c) { init(); }


MVImage::MVImage(const BaseImage& gi, int bs):
		blocksize(bs), BaseImage(gi)
{
	width /= blocksize; height /= blocksize;
	fullSize /= (blocksize*blocksize);
	size = fullSize;
	init();
}


MVImage::MVImage(const MVImage& mi, int a):
		blocksize(mi.blocksize), BaseImage(mi)
{
	init();
	if (!a) {
		copy(size, (unsigned char*) horzData,
				(unsigned char*) mi.horzData);
		copy(size, (unsigned char*) vertData,
				(unsigned char*) mi.vertData);
	}
} // end MVImage::MVImage()


MVImage::~MVImage()
{
	delete horzData; horzData = (char*) NULL;
	delete vertData; vertData = (char*) NULL;
}


BaseImage* MVImage::fragment(int cellSz, int Num) const
{
	int arrSz = cellSz / 2; // Since we have 2 buffers (horiz and vert)

	if ((Num == 0) && (size == 0)) {
		return ((BaseImage*) clone()); // NULL MV field.
	}
	if (Num*arrSz > size) { return ((BaseImage*) NULL); }

// Use clone(1) rather than clone() to avoid copying image data.
	MVImage* retval = (MVImage*) clone(1);
	retval->startPos = startPos + Num*arrSz;
	retval->size = min(startPos+size-retval->startPos, arrSz);
	delete retval->horzData; delete retval->vertData;
	retval->horzData = new char[retval->size];
	retval->vertData = new char[retval->size];

	int offset = retval->startPos - startPos;
	copy(retval->size, (unsigned char*) retval->horzData,
			(unsigned char*) horzData+offset);
	copy(retval->size, (unsigned char*) retval->vertData,
			(unsigned char*) vertData+offset);
	return(retval);
} // end MVImage::fragment()


void MVImage::assemble(const BaseImage* bi)
{
// Do we have an acceptable image to merge?
	if (!bi->isA("MVImage") || (*bi != *this) ) return;

// Are we set up to merge yet?
	if (size != fullSize) {
		char* tmpH = new char[fullSize];
		char* tmpV = new char[fullSize];
		for(int t = 0; t < fullSize; t++) { tmpH = tmpV = (char) 0; }
		copy(size, (unsigned char*) tmpH+startPos,
				(unsigned char*) horzData);
		copy(size, (unsigned char*) tmpV+startPos,
				(unsigned char*) vertData);
		delete horzData; delete vertData;
		horzData = tmpH; vertData = tmpV;
		size = fullSize;
	}

// Do the merge
	const MVImage* mi = (const MVImage*) bi;
	copy(mi->size, (unsigned char*) horzData+mi->startPos,
			(unsigned const char*) mi->horzData);
	copy(mi->size, (unsigned char*) vertData+mi->startPos,
			(unsigned const char*) mi->vertData);
} // end MVImage::assemble()

const char* MVImage::dataType() const { return("MVImage"); }
PacketData* MVImage::clone() const { return new MVImage(*this); }
PacketData* MVImage::clone(int a) const { return new MVImage(*this, a); }
ISA_FUNC(MVImage,BaseImage);

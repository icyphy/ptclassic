// filename:       GrayImage.cc
// author:         Paul Haskell
// creation date:  7/1/91
// SCCS info:      $Id$

#include "GrayImage.h"

void GrayImage::init()
{ grayData = new unsigned char[fullSize]; }


GrayImage::GrayImage(int a, int b, int c, int d):
        BaseImage(a, b, c, d) { init(); }


GrayImage::GrayImage(int a, int b, int c):
        BaseImage(a, b, c) { init(); }


GrayImage::GrayImage(BaseImage& bi):
        BaseImage(bi) { init(); }


GrayImage::GrayImage(GrayImage& gi):
        BaseImage(gi)
{
    grayData = new unsigned char[size];
    // the casts in the next line and all other calls to copy shouldn't
    // be necessary, but the current g++ gives warnings if they are absent.
    copy(size, (char*) grayData, (char*) gi.grayData);
} // end GrayImage::GrayImage()


GrayImage::~GrayImage()
{ delete grayData; grayData = (unsigned char*) NULL; }


BaseImage* GrayImage::fragment(int cellSz, int Num)
{
    if (Num*cellSz > size) return ((BaseImage*) NULL);

// The use of "bir" is a hack to get the GrayImage(BaseImage&) c'tor
// called when "retval" is created, rather than the
// GrayImage(GrayImage&) c'tor, which spends a lot of time copying
// memory.
    BaseImage& bir = *this;
    GrayImage* retval = new GrayImage(bir);
    retval->startPos = startPos + Num*cellSz;
    retval->size = min(startPos+size-retval->startPos, cellSz);
    delete retval->grayData; // some other way to do this?
    retval->grayData = new unsigned char[retval->size];

    int offset = retval->startPos - startPos;
    copy(retval->size, (char*) retval->grayData, (char*) grayData+offset);
    return(retval);
} // end GrayImage::fragment()


void GrayImage::assemble(BaseImage* bi)
{
// Do we have an acceptable image to merge?
    if (!StrStr(bi->dataType(), "GrayI") || (*bi != *this) ) return;

// Are we set up to merge yet?
    if (size != fullSize) {
        unsigned char* tmpPtr = new unsigned char[fullSize];
        copy(size, (char*) tmpPtr+startPos, (char*) grayData);
        delete grayData;
        grayData = tmpPtr;
        size = fullSize;
    }

// Do the merge
    GrayImage* gi = (GrayImage*) bi;
    copy(gi->size, (char*) grayData + gi->startPos, (char*) gi->grayData);
} // end GrayData::assemble()

// filename:       GrayImage.h
// author:         Paul Haskell
// creation date:  7/1/91
// SCCS info:      $Id$

#ifndef _GrayImage_h
#define _GrayImage_h

/*******************************
 Fragmentation is done in a raster-scanned order.
 'fullSize' should always equal (width*height);
 *******************************/

#include "BaseImage.h"

class GrayImage: public BaseImage {
protected:
    unsigned char* grayData;
    void init();

public:
    GrayImage(int, int, int, int); // width, height, frameId, fullSize
    GrayImage(int, int, int);      // width, height, frameId
    GrayImage(BaseImage&);         // get params from BaseImage
    GrayImage(GrayImage&);         // get params AND copy data
    virtual ~GrayImage();

    virtual BaseImage* fragment(int, int);
    virtual void       assemble(BaseImage*);
    unsigned char*     retData() { return grayData; }
    void               setSize(int a) { size = fullSize = a; }

// PacketData-like stuff
    virtual const char* dataType() const { return("GrayI.BaseI"); }
    virtual PacketData* clone() const { return new GrayImage(*this); }
};

#endif  // #ifndef _GrayImage_h

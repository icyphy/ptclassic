// filename:		GrayImage.h
// author:			Paul Haskell
// creation date:	7/1/91
// SCCS info:		$Id$

#ifndef _GrayImage_h
#define _GrayImage_h

/*******************************
 Fragmentation is done in a raster-scanned order.

 With the setSize() function, fullSize can vary from (width*height).
 This allows GrayImage picture data to be quantized, etc.
 *******************************/

#include "BaseImage.h"

class GrayImage: public BaseImage {
protected:
	unsigned char* grayData;
	void init();

public:
	GrayImage(int, int, int, int);	// width, height, frameId, fullSize
	GrayImage(int, int, int);		// width, height, frameId
	GrayImage(BaseImage&);			// get params from BaseImage
	GrayImage(GrayImage&, int = 0);	// get params, maybe copy data
	virtual ~GrayImage();

	virtual BaseImage*	fragment(int, int);
	virtual void		assemble(BaseImage*);
	unsigned char*		retData();
	void				setSize(int);

// PacketData-like stuff
	virtual const char* dataType() const { return("GrayI.BaseI"); }
	virtual PacketData* clone() const
			{ return new GrayImage(*this); }
	virtual PacketData* clone(int a) const
			{ return new GrayImage(*this, a); }
};

#endif // #ifndef _GrayImage_h

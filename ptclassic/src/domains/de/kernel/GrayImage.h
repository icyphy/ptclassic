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
	GrayImage(int, int, int, int); // width, height, frameId, fullSize
	GrayImage(int, int, int);      // width, height, frameId
	GrayImage(const BaseImage&);   // get params from BaseImage
	GrayImage(const GrayImage&, int = 0);	// get params, maybe copy data
	virtual ~GrayImage();

	virtual BaseImage*	fragment(int, int) const;
	virtual void		assemble(const BaseImage*);
	// retData gives a pointer you can write with
	unsigned char*		retData();

	// constData gives you a pointer you cannot write with;
	// it works for a const object
	unsigned const char*	constData() const { return grayData; }

	void			setSize(int);

// PacketData-like stuff
	virtual const char* dataType() const;
	virtual PacketData* clone() const;
	virtual PacketData* clone(int a) const;
	int isA(const char*) const;
};

#endif // #ifndef _GrayImage_h

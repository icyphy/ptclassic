// filename:		DCTImage.h
// author:			Sun-Inn Shih
// creation date:	7/11/91
// SCCS info:		$Id$

#ifndef _DCTImage_h
#define _DCTImage_h

/*****************************
 Fragmentation is done in raster-scanned order.

 'upWidth' and 'upHeight' are the same as width and height, but ROUNDED
 UP to the nearest multiple of blocksize--if width (height) is already a
 multiple of blocksize, upWidth (upHeight) equals width (height).
 ****************************/

#include <math.h>
#include "BaseImage.h"

class DCTImage: public BaseImage {
protected:
	int upWidth, upHeight;
	float* DCTData;
	int blocksize;
	void init();

public:
	DCTImage(int, int, int, int, int);	// width, height, frameId,
										// fullSize, blocksize
	DCTImage(int, int, int, int = 8);	// width, height, frameId,
										// blocksize
	DCTImage(DCTImage&, int = 0);		// copy params, maybe data
	DCTImage(BaseImage&, int = 8); // copy params, int = blocksize
	virtual ~DCTImage();

	virtual	BaseImage*	fragment(int, int);
	virtual	void		assemble(BaseImage*);

	inline	float* retData()		{ return DCTData; }
	inline	int	retBS()				{ return blocksize; }
	inline	int	fullWidth()			{ return upWidth; }
	inline	int	fullHeight()		{ return upHeight; }

// PacketData-like stuff
	virtual const char* dataType() const { return("DCTI.BaseI"); }
	virtual PacketData* clone() const
			{ return new DCTImage(*this); }
	virtual PacketData* clone(int a) const
			{ return new DCTImage(*this, a); }
};

#endif // #ifndef _DCTImage_h

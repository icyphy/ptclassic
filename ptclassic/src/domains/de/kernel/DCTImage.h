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

 With the setSize() function, fullSize can vary from
 (fullWidth*fullHeight). This allows DCTImage picture data to be
 compressed, etc.
 ****************************/

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
	DCTImage(const DCTImage&, int = 0); // copy params, maybe data
	DCTImage(const BaseImage&, int = 8); // copy params, int = blocksize
	virtual ~DCTImage();

	virtual	BaseImage*	fragment(int, int) const;
	virtual	void		assemble(const BaseImage*);
	float*				retData();

// For const packets.
	const float*		constData() const { return DCTData; }

	void				setSize(const int);

	inline	int	retBS() const
			{ return blocksize; }
	inline	int	fullWidth() const
			{ return upWidth; }
	inline	int	fullHeight() const
			{ return upHeight; }
	inline	int processed() const
			{ return(upWidth*upHeight != fullSize); }

// PacketData-like stuff
	virtual const char* dataType() const;
	virtual PacketData* clone() const;
	virtual PacketData* clone(int a) const;
	int isA(const char*) const;
};

#endif // #ifndef _DCTImage_h

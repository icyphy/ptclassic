// filename:		MVImage.h
// author:			Paul Haskell
// creation date:	7/10/91
// SCCS info:		$Id$

#ifndef _MVImage_h
#define _MVImage_h

/***************************
 Store a frame's worth of motion vectors.  Store motion vectors
 as two signed char's:  one for horiz displacement and one for
 vert displacement.

 Fragmented in raster-scanned order.

 A "null" motion vector field, i.e. width = height = size = 0,
 is used to tell the decoder that the difference frame is
 INTRAFRAME coded--no motion compensation is done.  A null mot. vec.
 field can be used at the start of a sequence or for
 resynchronization.

 The "width" and "height" are for a frame of motion vectors, not for
 the frame of pixels they came from.  So in the BaseImage& constructor:
 width = frameWidth/blocksize, height = frameHeight/blocksize,
 size = frameSize / (blocksize^2)
 *************************/

#include "BaseImage.h"

class MVImage: public BaseImage {
protected:
	void	init();
	char*	horzData;
	char*	vertData;
	int		blocksize;

public:
	MVImage();		// dummy MVImage.
	MVImage(int, int, int, int, int); // width, height, frameId,
					  // fullSize, blocksize
	MVImage(int, int, int, int = 8);  // width, height, frameId,
					  // blocksize
	MVImage(const BaseImage&, int = 8); // copy params, int = blocksize
	MVImage(const MVImage&, int = 0);   // copy params, maybe data
	virtual ~MVImage();

	virtual	BaseImage*	fragment(int, int) const;
	virtual	void		assemble(const BaseImage*);

	inline	char*	retHorz() { return horzData; }
	inline	char*	retVert() { return vertData; }

	// versions for const images
	inline	const char* constHorz() const { return horzData; }
	inline	const char* constVert() const { return vertData; }


	inline	int		retBlockSize() const	{ return blocksize; }
	inline	int		bsMatch(MVImage* mv) const
			{ return (blocksize == mv->blocksize); }

// Message-like stuff
	virtual const char* dataType() const;
	virtual Message* clone() const;
	virtual Message* clone(int a) const;
	int isA(const char*) const;
};

#endif // #ifndef _MVImage_h

/* 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY
*/
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

// For const images.
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

// Message-like stuff
	virtual const char* dataType() const;
	virtual Message* clone() const;
	virtual Message* clone(int a) const;
	int isA(const char*) const;
};

#endif // #ifndef _DCTImage_h

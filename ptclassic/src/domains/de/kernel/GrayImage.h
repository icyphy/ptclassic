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

// Message-like stuff
	virtual const char* dataType() const;
	virtual Message* clone() const;
	virtual Message* clone(int a) const;
	int isA(const char*) const;
};

#endif // #ifndef _GrayImage_h

// filename:		BaseImage.h
// author:			Paul Haskell
// creation date:	7/1/91
// SCCS info:		$Id$

#ifndef _BaseImage_h
#define _BaseImage_h
#ifdef __GNUG__
#pragma interface
#endif

/*******************************
 This file defines a base class for all image types in Ptolemy.
 The BaseImage type is based on Ptolemy's built-in "PacketData"
 type, which allows aggregate datatypes to be passed between
 Stars.

 The fragment() method partitions an image into many smaller images,
 which together represent the same picture as the original.
 The assemble() method combines many small images which make up a
 single picture into a single image that contains the picture.
 fragment() should work recursively, so an image that has
 been produced by a previous fragment() call can be further fragmented.
 Assembly always produces a full-sized image from however-small
 fragments.

 The 'frameId' variable is used during assembly.  Fragments with the
 same frameId's are assembled into the same image.  So, it is IMPORTANT
 that different frames from the same source have different frameIds.

 Use of the 'size', 'fullSize', and 'startPos' variables vary within
 each subclass.  Typically the 'size' variable holds the number of
 pixels that an object is storing.  If an object is NOT produced by
 fragment(), then (size == fullSize).  If the object is produced by a
 fragment() call, size may be less than or equal to fullSize.
 An objects's 'fullSize' may be bigger or
 smaller than width*height.  It would be bigger, for example, in
 DCTImage, where the amount of allocated storage must be rounded up to
 be a multiple of the blocksize.  It would be smaller, for example,
 for an object that contains runlength coded video.

 The comparison functions {==, !=, <, >, etc.} compare two objects'
 frameId's.  They can be used to resequence images or to sort image
 fragments.

 There is a GNU g++ compiler bug that prevents us from writing
 a function copy(int, char*, char*).  This function would conflict
 (erroneously) with the copy(int, unsigned char*, unsigned char*)
 function.  Until this bug is fixed, just CAST char*'s to unsigned
 char*'s before calling copy().

 The copy constructor and clone() functions have an optional integer
 argument.  If a nonzero argument is provided, then all state values
 of the copied object are copied to the created object, but none
 of the image data is copied.  If no argument or a zero argument
 is provided, then the image data is copied as well.  Classed derived
 from BaseImage should maintain this policy.
 ******************************/

#include "Packet.h"
#include <builtin.h>

class BaseImage: public PacketData {
protected:
	int width, height;
	int startPos, size, fullSize;
	int frameId;
	void copy(int, float*, const float*) const;
	void copy(int, unsigned char*, const unsigned char*) const;

public:
	BaseImage(int a, int b, int c, int d):
			width(a), height(b), frameId(c), fullSize(d)
			{ startPos = 0; size = fullSize; }

	BaseImage(int a, int b, int c):
			width(a), height(b), frameId(c)
			{ startPos = 0; size = fullSize = width*height; }

	BaseImage(const BaseImage& bi, int = 0):
			width(bi.width), height(bi.height), frameId(bi.frameId),
			startPos(bi.startPos), size(bi.size), fullSize(bi.fullSize)
			{ }

	virtual ~BaseImage() { ; }

	inline int retWidth()		const { return(width); }
	inline int retHeight()		const { return(height); }
	inline int retFullSize()	const { return(fullSize); }
	inline int retSize()		const { return(size); }
	inline int retId()			const { return(frameId); }

	inline int operator==(const BaseImage& a) const
			{ return (frameId == a.frameId); }
	inline int operator!=(const BaseImage& a) const
			{ return (!(*this == a)); }
	inline int operator<(const BaseImage& a) const
			{ return (frameId < a.frameId); }
	inline int operator>(const BaseImage& a) const
			{ return (a < *this); }
	inline int operator<=(const BaseImage& a) const
			{ return ((*this < a) || (*this == a)); }
	inline int operator>=(const BaseImage& a) const
			{ return ((*this > a) || (*this == a)); }

	virtual BaseImage* fragment(int, int) const;
	virtual void assemble(const BaseImage*);

// PacketData-like stuff
	virtual const char* dataType() const;
	virtual PacketData* clone() const;
	virtual PacketData* clone(int a) const;
	int isA(const char*) const;
};

#endif // #ifndef _BaseImage_h

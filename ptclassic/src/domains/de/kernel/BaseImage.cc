// filename:		BaseImage.cc
// author:			Paul Haskell
// creation date:	7/1/91
// SCCS info:		$Id$


#include "BaseImage.h"

BaseImage* BaseImage::fragment(int, int) const { return 0;}
void BaseImage::assemble(const BaseImage*) {}
const char* BaseImage::dataType() const { return "BaseImage";}
PacketData* BaseImage::clone() const { return new BaseImage(*this); }
PacketData* BaseImage::clone(int a) const { return new BaseImage(*this,a);}

void BaseImage::copy(int len, float* into, const float* from) const
{
	for(int travel = 0; travel < len%5; travel++) {
		into[travel] = from[travel];
	}
	for(travel = len%5; travel < len; travel+=5) {
		into[travel] = from[travel];
		into[travel+1] = from[travel+1];
		into[travel+2] = from[travel+2];
		into[travel+3] = from[travel+3];
		into[travel+4] = from[travel+4];
	}
}


void BaseImage::copy(int len, unsigned char* into,
		const unsigned char* from) const
{
	for(int travel = 0; travel < len%5; travel++) {
		into[travel] = from[travel];
	}
	for(travel = len%5; travel < len; travel+=5) {
		into[travel] = from[travel];
		into[travel+1] = from[travel+1];
		into[travel+2] = from[travel+2];
		into[travel+3] = from[travel+3];
		into[travel+4] = from[travel+4];
	}
} // end BaseImage::copy()

ISA_FUNC(BaseImage,PacketData);

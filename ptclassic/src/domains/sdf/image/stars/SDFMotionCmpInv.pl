defstar {
	name		{ MotionCmpInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 Regents of the University of California }
	location	{ SDF image palette }
	desc {
For NULL inputs on 'mvIn' (motion-vector-in), copy the 'diffIn' input
unchanged to the output and discard the 'pastIn' input.
(A NULL input usually indicates the first frame of a sequence.)

For non-NULL 'mvIn' inputs, perform inverse motion compensation and
write the result to 'output'.
}


//////// I/O AND STATES.
	input {		name { diffIn }	type { packet } }
	input {		name { pastIn }	type { packet } }
	input {		name { mvIn }	type { packet } }
	output {	name { output }	type { packet } }


////// CODE.
	hinclude { "GrayImage.h", "MVImage.h", "Error.h" }

	method {
		// Return 1 if all inputs are read ok. Return 0 if this is an
		// intraframe-coded input, i.e. size of mvImage is 0 OR
		// pastImage is a NULL packet. Handle the ERROR if diffImage or
		// mvImage is of incorrect type.
		name { otherInputs }
		type { "int" }
		arglist { "(const GrayImage** pi, const MVImage** mi, \
					Packet& pPkt, Packet& mPkt)" }
		access { private }
		code {
			(pastIn%0).getPacket(pPkt);
			(mvIn%0).getPacket(mPkt);

			if (badType(*this, mPkt, "MVImage")) { return 0; }
			*mi = (const MVImage*) mPkt.myData();
			if (!(**mi).retSize()) { return 0; }

			if (pPkt.typeCheck("GrayImage")) {
				*pi = (const GrayImage*) pPkt.myData();
				return 1;
			}
			return 0;
		}
	} // end otherInputs()


	method {
		name { doInvMC }
		type { "void" }
		access { private }
		arglist { "(unsigned char* out, unsigned const char* prev, \
				unsigned const char* diff, const char* horz, \
				const char* vert, const int width, \
				const int height, const int blocksize)" }
		code {
			int ii, jj;
			for(ii = 0; ii < height; ii += blocksize) {
				for(jj = 0; jj < width; jj += blocksize) {
					if(BlockIsLost(diff, ii, jj, blocksize, width)) {
						DoLostBlock(out, prev, *horz, *vert, ii,
								jj, blocksize, width);
					} else {
						DoOneBlock(out, prev, diff, *horz, *vert, ii,
								jj, blocksize, width);
					}
					horz++; vert++;
			}	}
		}
	} // end doInvMC()

	method {
		name { BlockIsLost }
		access { private }
		arglist{ "(unsigned const char* ptr, const int ii, \
				const int jj, const int blocksize, const int width)" }
		type { "int" }
		code {
			int i, j, indx;
			for(i = 0; i < blocksize; i++) {
				indx = jj + (i+ii)*width;
				for(j = 0; j < blocksize; j++) {
					if (ptr[indx + j] != (unsigned char) 0) {
						return 0;
			}	}	}
			return 1;
		}
	}

	virtual method {
		name { DoOneBlock }
		type { "void" }
		access { protected }
		arglist { "(unsigned char* out, unsigned const char* prev, \
				unsigned const char* diff, const char horz, \
				const char vert, const int ii, const int jj, \
				const int blocksize, const int width)" }
		code {
			int i, j, index, mcindex;
			for(i = 0; i < blocksize; i++) {
				index = jj + (i+ii)*width;
// NOTE THE MINUS '-' SIGNS BELOW. These are because the
// motion vector points FROM the old block TO the current.
				mcindex = index - int(horz) - (width * int(vert));
				for(j = 0; j < blocksize; j++) {
					out[index+j] = quant(diff[index+j]+prev[mcindex+j]);
			}	}
		}
	} // end DoOneBlock{}

	virtual method {
		name { DoLostBlock }
		type { "void" }
		access { protected }
		arglist { "(unsigned char* out, unsigned const char* prev, \
				const char horz, const char vert, const int ii, \
				const int jj, const int blocksize, const int width)" }
		code {
			int i, j, index, mcindex;
			for(i = 0; i < blocksize; i++) {
				index = jj + (i+ii)*width;
// NOTE THE MINUS '-' SIGNS BELOW. These are because the
// motion vector points FROM the old block TO the current.
				mcindex = index - int(horz) - (width * int(vert));
				for(j = 0; j < blocksize; j++) {
					out[index+j] = prev[mcindex+j];
			}	}
		}
	} // end DoLostBlock{}

	inline virtual method {
		name { quant }
		type { "unsigned char" }
		access { protected }
		arglist { "(const float in)" }
		code { return((unsigned char) (in - 127.5)); }
	}

	method {
		name { sizesMatch }
		type { "int" }
		access { private }
		arglist { "(const GrayImage* im1, const GrayImage* im2, \
				const MVImage* imm)" }
		code {
			int w = im1->retWidth();
			int h = im1->retHeight();
			int retval = (w == im2->retWidth());
			retval &= (h == im2->retHeight());
			int bs = imm->retBlockSize();
			retval &= (w == bs*(w/bs));
			retval &= (h == bs*(h/bs));
			return(retval);
		}
	}

	go {
// Read data from inputs and initialize if this is the a resync.
		Packet difPkt, pastPkt, mvPkt; // go out of scope at end of go{}
		const GrayImage* diffImg;
		const GrayImage* pastImg;
		const MVImage* mvImg;

		(diffIn%0).getPacket(difPkt);
		TYPE_CHECK(difPkt, "GrayImage");
		diffImg = (const GrayImage*) difPkt.myData();

// Handle resynchronization inputs.
		if (!otherInputs(&pastImg, &mvImg, pastPkt, mvPkt)) {
			Packet out(*diffImg); output%0 << out;
			return;
		}

		if(!sizesMatch(diffImg, pastImg, mvImg)) {
			Error::abortRun(*this, "Size mismatch");
			return;
		}

// If we reached here, we have a motion-compensated image.
// Create output image. Call clone(), which is virtual, so we preserve
// type and state info from the input difference image.
// Use clone(int) rather than clone() to avoid copying data.
		GrayImage* outImage = (GrayImage*) diffImg->clone(1);

// Do the inverse motion compensation and send output.
		doInvMC(outImage->retData(), pastImg->constData(),
				diffImg->constData(), mvImg->constHorz(),
				mvImg->constVert(), diffImg->retWidth(),
				diffImg->retHeight(), mvImg->retBlockSize());

		Packet outPkt(*outImage); output%0 << outPkt;
	} // end go{}
} // end defstar { MotionCmpInv }

defstar {
	name		{ MotionCmpInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
For inputs with NULL motion vector frames (e.g. the first frame of
a sequence), copy the difference unchanged to the output.

For all other inputs, perform inverse motion compensation and write the
result to the output.

}

	hinclude { "GrayImage.h", "MVImage.h" }

//////// I/O AND STATES.
	input {		name { diffIn }	type { packet } }
	input {		name { mvIn }	type { packet } }
	output {	name { output }	type { packet } }

////// CODE.
	protected {
		Packet storPkt;
		int width, height;
	}


	method {
		name { getInputs }
		type { "void" }
		arglist { "(const GrayImage** gi, const MVImage** mi,
				Packet& inPkt, Packet& mvPkt)" }
		access { protected }
		code {
			(diffIn%0).getPacket(inPkt);
			(mvIn%0).getPacket(mvPkt);
			TYPE_CHECK(inPkt, "GrayImage");
			TYPE_CHECK(mvPkt, "MVImage");
			*gi = (const GrayImage*) inPkt.myData();
			*mi = (const MVImage*) mvPkt.myData();
		}
	} // end getInputs()


	virtual method {
		name	{ doIntraImage }
		type	{ "void" }
		access	{ protected }
		arglist { "(const GrayImage* gi)" }
		code {
			width	= gi->retWidth();
			height	= gi->retHeight();
			Packet pkt(*gi);
			output%0 << pkt;
		}
	} // end doIntraImage()


	virtual method {
		name { quant }
		type { "unsigned char" }
		access { protected }
		arglist { "(const float in)" }
		code { return((unsigned char) (in - 127.5)); }
	}


	method {
		name { doInvMC }
		type { "void" }
		access { protected }
		arglist { "(unsigned char* out, unsigned const char* prev,
				unsigned const char* diff, const char* horz,
				const char* vert, const int width,
				const int height, const int blocksize)" }
		code {
			int ii, jj;
			for(ii = 0; ii < height; ii += blocksize) {
				for(jj = 0; jj < width; jj += blocksize) {
					DoOneBlock(out, prev, diff, *horz, *vert, ii, jj,
							blocksize);
					horz++; vert++;
			}	}
		}
	} // end doInvMC()


	virtual method {
		name { DoOneBlock }
		type { "void" }
		access { protected }
		arglist { "(unsigned char* out, unsigned const char* prev,
				unsigned const char* diff, const char horz,
				const char vert, const int ii, const int jj,
				const int blocksize)" }
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


	go {
// Read data from inputs.
		Packet pastPkt = storPkt; // Holds result of previous run.
		Packet mvPkt; // goes out of scope at end of go{}
		GrayImage*	diffImage;
		MVImage*	mvImage;
		getInputs(&diffImage, &mvImage, storPkt, mvPkt);

// Initialize if this is the a resynchronization.
		if (! mvImage->retSize()) { // then intraframe coded image.
			doIntraImage(diffImage);
			return;
		}

// If we reached here, we have a motion-compensated image.
// Create output image. Call clone(), which is virtual, so we preserve
// type and state info from the input difference image.
// Use clone(int) rather than clone() to avoid copying data.
		GrayImage* outImage = (GrayImage*) diffImage->clone(1);
		const GrayImage* prvImage = (const GrayImage*) pastPkt.myData();

////// Do the inverse motion compensation.
		doInvMC(outImage->retData(), prvImage->constData(),
				diffImage->constData(), mvImage->constHorz(),
				mvImage->constVert(), width, height,
				mvImage->retBlockSize());

// Save the state and send the outputs on their way.
		Packet tmpPkt(*outImage); // Save current frame in storPkt.
		storPkt = tmpPkt;
		Packet outPkt(*outImage); output%0 << outPkt;

// pastPkt goes out of scope, decrementing its contents' ref. count.
	} // end go{}
} // end defstar { MotionCmpInv }

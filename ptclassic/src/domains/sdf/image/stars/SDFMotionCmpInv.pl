defstar {
	name		{ MotionCmpInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
For the first inputs, copy the difference unchanged to the output.

For all other inputs, perform inverse motion compensation and write the
result to the output.
	}

code {
//
// Notice all the games we play with variables of type "Packet" to get
// the PacketData objects (GrayImage and MVImage) to stick around for
// just the right amount of time.  Once a PacketData object is stuck
// into a Packet, the PacketData object lives only until:  the packet
// variable goes out of scope, something else is copied into the Packet
// variable.
//
}

	hinclude { "GrayImage.h", "MVImage.h", "Error.h" }

//////// I/O AND STATES.
	input {
		name { diffIn }
		type { packet }
	}
	input {
		name { mvIn }
		type { packet }
	}
	output {
		name { output }
		type { packet }
	}

////// CODE.
	protected {
		Packet storPkt;
		int width, height;
	}

	method {
		name { getInputs }
		type { "void" }
		arglist { "(GrayImage** gi, MVImage** mi, Packet& inPkt,
				Packet& mvPkt)" }
		access { protected }
		code {
			(diffIn%0).getPacket(inPkt);
			(mvIn%0).getPacket(mvPkt);
			TYPE_CHECK(inPkt,"GrayImage");
			TYPE_CHECK(mvPkt,"MVImage");
			*gi = (GrayImage*)	inPkt.myData();
			*mi = (MVImage*)	mvPkt.myData();
		}
	} // end getInputs()

	method {
		name	{ doIntraImage }
		type	{ "void" }
		access	{ protected }
		arglist { "(GrayImage* gi)" }
		code {
			width  = gi->retWidth();
			height = gi->retHeight();
			Packet pkt(*gi);
			output%0 << pkt;
		}
	} // end doIntraImage()

	method {
		name { doInvMC }
		type { "void" }
		access { protected }
		arglist { "(unsigned char* out, unsigned char* prev,
				unsigned char* diff, char* horz, char* vert,
				int width, int height, int blocksize)" }
		code {
		int i, j, ii, jj, index, mcindex;
		for(ii = 0; ii < height; ii += blocksize) {
			for(jj = 0; jj < width; jj += blocksize) {
				for(i = 0; i < blocksize; i++) {
					index = jj + (i+ii)*width;
					mcindex = index + int(*horz) + (width*int(*vert));
					for(j = 0; j < blocksize; j++) {
						out[index+j] = diff[index+j] + prev[mcindex+j];
					}
				}
				horz++; vert++;
			}
		}
		}
	} // end doInvMC()

	go {
// Read data from inputs.
		Packet pastPkt = storPkt; // Holds result of previous run.
		Packet mvPkt; // goes out of scope at end of go{}
		GrayImage* diffImage;
		MVImage*   mvImage;
		getInputs(&diffImage, &mvImage, storPkt, mvPkt);

// Initialize if this is the a resynchronization.
		if (! mvImage->retSize()) { // then intraframe coded image.
			doIntraImage(diffImage);
			return;
		}

// If we reached here, we have a motion-compensated image.
// Create output image.  Call clone(), which is virtual, so we preserve
// type and state info from the input difference image.
// Use clone(int) rather than clone() to avoid copying data.
		GrayImage* outImage = (GrayImage*) diffImage->clone(1);

////// Do the inverse motion compensation.
		GrayImage* prevImage = (GrayImage*) pastPkt.myData();
		doInvMC(outImage->retData(), prevImage->retData(),
				diffImage->retData(), mvImage->retHorz(),
				mvImage->retVert(), width, height,
				mvImage->retBlockSize());

// Save the state and send the outputs on their way.
		Packet tmpPkt(*outImage); // Save current frame in storPkt.
		storPkt = tmpPkt;
		Packet outPkt(*outImage); output%0 << outPkt;
// pastPkt goes out of scope now, decrementing its contents' reference
// count.
	} // end go{}
} // end defstar { MotionCmpInv }

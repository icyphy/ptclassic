defstar {
	name		{ MotionCmp }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
For the first input image, copy the image unchanged to the diffOut
output and send a null field of motion vectors to the mvOut output.

For all other inputs, perform motion compensation and write the
difference frames and motion vector frames to the corresponding outputs.

This star can be derived from, to implement slightly different motion
compensation algorithms. For example, synchronization techniques
can be added.
}

	hinclude { "GrayImage.h", "MVImage.h", "Error.h" }

//////// I/O AND STATES.
	input {		name { input }		type { packet } }
	output {	name { diffOut }	type { packet } }
	output {	name { mvOut }		type { packet } }

	defstate {
		name	{ blockSize }
		type	{ int }
		default { 8 }
		desc	{ Size of blocks on which to do motion comp. }
	}

////// CODE.
	protected {
		Packet storPkt;
		int firstTime; // True if we have not received any inputs yet.
		int width, height, blocksize;
	}


	start {
		firstTime = 1;
		blocksize = int(blockSize);
	}


	method {
		name { getInput }
		type { "const GrayImage*" }
		access { protected }
		arglist { "(Packet& inPkt)" }
		code {
			(input%0).getPacket(inPkt);
			if (badType(*this, inPkt, "GrayImage")) {
				return NULL;
			}
			return( (const GrayImage*) inPkt.myData());
		}
	} // end getInput()


	virtual method {
		name	{ doFirstImage }
		access	{ protected }
		type	{ "void" }
		arglist	{ "(const GrayImage* inp)" }
		code {
			width	= inp->retWidth();
			height	= inp->retHeight();
			if ( (width != blocksize * (width/blocksize)) ||
					(height	!= blocksize * (height/blocksize)) ) {
				Error::abortRun(*this, "block vs. image size error.");
				return;
			}

			LOG_NEW; MVImage* mv = new MVImage;
			Packet pkt(*inp), empty(*mv);
			diffOut%0 << pkt;	// write first input unchanged.
			mvOut%0 << empty;
			firstTime = 0;
		}
	} // end doFirstImage()


	virtual method { // don't do thresholding, use wrap-around.
		name { quant }
		access { protected }
		type { "unsigned char" }
		arglist { "(const float inp)"}
		code { return((unsigned char) (inp + 0.5)); }
	}


	method {
		name { doMC }
		type { "void" }
		access { protected }
		arglist { "(unsigned char* diff, unsigned const char* cur,
			unsigned const char* prev, char* horz, char* vert)" }
		code {
			int ii, jj, xvec, yvec;
	
			// Do top row.
			for(jj = 0; jj < width; jj += blocksize) {
				DoOneBlock(*horz, *vert, diff, cur, prev, 0, jj, 0, 0);
				horz++; vert++;
			}
	
			// Do middle rows.
			for(ii = blocksize; ii < height-blocksize; ii+=blocksize) {
				DoOneBlock(*horz, *vert, diff, cur, prev, ii, 0, 0, 0);
				horz++; vert++;
	
				for(jj = blocksize; jj < width-blocksize;
						jj += blocksize) {
					FindMatch(cur, prev, ii, jj, xvec, yvec);
					DoOneBlock(*horz, *vert, diff, cur, prev, ii, jj,
							xvec, yvec);
					horz++; vert++;
				}
	
				DoOneBlock(*horz, *vert, diff, cur, prev, ii,
						width-blocksize, 0, 0);
				horz++; vert++;
			} // end middle rows
	
			// Do bottom row.
			for(jj = 0; jj < width; jj += blocksize) {
				DoOneBlock(*horz, *vert, diff, cur, prev,
						height-blocksize, jj, 0, 0);
				horz++; vert++;
			}
		} // end code{}
	} // end doMC()


	virtual method {
		name { FindMatch }
		type { "void" }
		arglist { "(unsigned const char* cur, unsigned const char* prev,
				const int ii, const int jj, int& xvec, int& yvec)" }
		access { protected }
		code {
			int i, j, deli, delj, *diffArr, bs2 = 2 * blocksize;
			register int tmp1, tmp2, tmp3;
			LOG_NEW; diffArr = new int[bs2*bs2];

		// Set difference values for each offset
			for(deli = 0; deli < bs2; deli++) {
				for(delj = 0; delj < bs2; delj++) {
					tmp3 = deli*bs2 + delj;
					diffArr[tmp3] = 0;
					for(i = 0; i < blocksize; i++) {
						tmp1 = (ii+i)*width + jj;
						tmp2 = (ii+i+deli-blocksize)*width +
								jj+delj-blocksize;
						for(j = 0; j < blocksize; j++) {
							diffArr[tmp3] += abs(int(cur[tmp1+j]) -
									int(prev[tmp2+j]));
			}	}	}	}

		// Find min difference
			int mini, minj;
			mini = minj = blocksize;
			for(i = 0; i < bs2; i++) {
				for(j = 0; j < bs2; j++) {
					if (diffArr[bs2*i+j] < diffArr[bs2*mini+minj]) {
						mini = i; minj = j;
			}	}	}
			yvec = mini - blocksize; xvec = minj - blocksize;
			LOG_DEL; delete diffArr;
		}
	} // end FindMatch{}


	virtual method {
		name { DoOneBlock }
		access { protected }
		type { "void" }
		arglist { "(char& horz, char& vert, unsigned char* diff,
				unsigned const char* cur, unsigned const char* prev,
				const int ii, const int jj, const int xvec,
				const int yvec)" }
		code { // Set diff frame and mvects.
			int i, j, tmp1, tmp2;
			for(i = 0; i < blocksize; i++) {
				tmp1 = (ii+i+yvec)*width + jj+xvec;
				tmp2 = (ii+i)*width + jj;
				for(j = 0; j < blocksize; j++) {
					diff[tmp2+j] = quant(cur[tmp2+j] - prev[tmp1+j]);
			}	}
// NOTE THE -1's!! These are so the motion vector points FROM the past
// block TO the current block, rather than the other way around!
			horz = char(-1*xvec); vert = char(-1*yvec);

		}
	} // end DoOneBlock{}


	go {
// Read data from input.
		Packet prevPkt = storPkt; // Holds data from last iteration.
		const GrayImage* inImage =
				getInput(storPkt); // New data in storPkt.
		if (inImage == NULL) {
			Error::abortRun(*this, "Can not read input image.");
			return;
		}

// Initialize if this is the first input image.
		if (firstTime) {
			doFirstImage(inImage);
			return;
		}

// If we reached here, we already have read one image.
// Create difference and mvect images.
// clone() is overloaded, so if inImage is of a type DERIVED from
// GrayImage, outImage will be of the same derived type.
// Use clone(int) rather than clone() so we don't copy image data.
		GrayImage*	outImage	= (GrayImage*) inImage->clone(1);
		const GrayImage* prvImage = (const GrayImage*) prevPkt.myData();
		LOG_NEW; MVImage*	mvImage		= new MVImage(*outImage, blocksize);

////// Do the motion compensation.
		doMC(outImage->retData(), inImage->constData(),
				prvImage->constData(), mvImage->retHorz(),
				mvImage->retVert());

// Send the outputs on their way.
		Packet diffPkt(*outImage); diffOut%0 << diffPkt;
		Packet mvPkt(*mvImage);	mvOut%0 << mvPkt;
// pastPkt goes out of scope, and its contents' reference count is
// decremented now.
	} // end go{}
} // end defstar { MotionCmp }

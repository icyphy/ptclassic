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
	}

	hinclude { "GrayImage.h", "MVImage.h", "Error.h" }

//////// I/O AND STATES.
	input {
		name { input }
		type { packet }
	}
	output {
		name { diffOut }
		type { packet }
	}
	output {
		name { mvOut }
		type { packet }
	}
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
		type { "GrayImage*" }
		access { protected }
		arglist { "(Packet& inPkt)" }
		code {
			(input%0).getPacket(inPkt);
			if (!StrStr(inPkt.dataType(), "GrayI")) {
				Error::abortRun(*this, inPkt.typeError("GrayI"));
				return((GrayImage*) NULL);
			}
			return( (GrayImage*) inPkt.myData());
		}
	} // end getInput()

	method {
		name	{ doFirstImage }
		access	{ protected }
		type	{ "void" }
		arglist	{ "(GrayImage* inp)" }
		code {
			width  = inp->retWidth();
			height = inp->retHeight();
			if ( (width  != blocksize * (width / blocksize)) ||
				 (height != blocksize * (height / blocksize)) ) {
					Error::abortRun(*this, "Bad block size.");
			}

			MVImage* mv = new MVImage;
			Packet pkt(*inp), empty(*mv);
			diffOut%0 << pkt;	// write first input unchanged.
			mvOut%0 << empty;
			firstTime = 0;
		}
	} // end doFirstImage()

	method {
		name { doMC }
		type { void }
		access { protected }
		arglist { "(unsigned char* diff, unsigned char* cur,
				unsigned char* prev, char* horz, char* vert)" }
		code {
		int i, j, ii, jj, index;

		// Do top row.
		for(jj = 0; jj < width; jj += blocksize) {
			for(i = 0; i < blocksize; i++) {
				index = jj + i*width;
				for(j = 0; j < blocksize; j++) {
					diff[index+j] = cur[index+j] - prev[index+j];
				}
			}
			*horz = *vert = 0; horz++; vert++;
		}

		// Do middle rows.
		for(ii = blocksize; ii < height-blocksize; ii += blocksize) {
			// Do leftmost block
			for(i = 0; i < blocksize; i++) {
				index = (ii+i)*width;
				for(j = 0; j < blocksize; j++) {
					diff[index+j] = cur[index+j] - prev[index+j];
				}
			}
			*horz = *vert = 0; horz++; vert++;

			// Do middle blocks
			for(jj = blocksize; jj < width-blocksize; jj += blocksize) {
				setMvDiff(horz, vert, diff, cur, prev, ii, jj);
				horz++; vert++;
			}

			// Do rightmost block
			for(i = 0; i < blocksize; i++) {
				index = (ii+i)*width;
				for(j = width-blocksize; j < width; j++) {
					diff[index+j] = cur[index+j] - prev[index+j];
				}
			}
			*horz = *vert = 0; horz++; vert++;
		} // end middle rows

		// Do bottom row.
		for(jj = 0; jj < width; jj += blocksize) {
			for(i = height-blocksize; i < height; i++) {
				index = jj + i*width;
				for(j = 0; j < blocksize; j++) {
					diff[index+j] = cur[index+j] - prev[index+j];
				}
			}
			*horz = *vert = 0; horz++; vert++;
		}

		} // end code{}
	} // end doMC()

	method {
		name { setMvDiff }
		type { "void" }
		arglist { "(char* horz, char* vert, unsigned char* diff,
				unsigned char* cur, unsigned char* prev, int ii,
				int jj)" }
		access { protected }
		code {
			int i, j, deli, delj, *diffArr, bs2 = 2 * blocksize;
			register int tmp1, tmp2, tmp3;
			diffArr = new int[bs2*bs2];

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
			}   }   }   }

		// Find min difference
			int mini, minj;
			mini = minj = blocksize;
			for(i = 0; i < bs2; i++) {
				for(j = 0; j < bs2; j++) {
					if (diffArr[bs2*i+j] < diffArr[bs2*mini+minj]) {
						mini = i; minj = j;
			}   }   }
			mini -= blocksize; minj -= blocksize;

		// Set mvects and diff frame
			*horz = char(minj); *vert = char(mini);
			for(i = 0; i < blocksize; i++) {
				tmp1 = (ii+i+mini)*width + jj+minj;
				tmp2 = (ii+i)*width + jj;
				for(j = 0; j < blocksize; j++) {
					diff[tmp2+j] = cur[tmp2+j] - prev[tmp1+j];
			}   }

			delete diffArr;
		}
	} // end setMvDiff{}

	go {
// Read data from input.
		Packet prevPkt = storPkt; // Holds data from last iteration.
		GrayImage* inImage = getInput(storPkt); // New data in storPkt.

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
		GrayImage* outImage = (GrayImage*) inImage->clone(1);
		MVImage*   mvImage  = new MVImage(*outImage, blocksize);

////// Do the motion compensation.
		GrayImage* prevImage = (GrayImage*) prevPkt.myData();
		doMC(outImage->retData(), inImage->retData(),
				prevImage->retData(), mvImage->retHorz(),
				mvImage->retVert());

// Send the outputs on their way.
		Packet diffPkt(*outImage); diffOut%0 << diffPkt;
		Packet mvPkt(*mvImage);	mvOut%0 << mvPkt;
// pastPkt goes out of scope, and its contents' reference count is
// decremented now.
	} // end go{}
} // end defstar { MotionCmp }

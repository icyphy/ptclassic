defstar {
	name		{ MotionCmp }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ SDF image palette }
	desc {
If the 'past' input is not a GrayImage (e.g. 'nullPacket'), copy
the 'input' image unchanged to the 'diffOut' output and send a null
field of motion vectors to the 'mvOut' output. This should usually
happen only on the first firing of the star.

For all other inputs, perform motion compensation and write the
difference frames and motion vector frames to the corresponding outputs.

This star can be derived from, to implement slightly different motion
compensation algorithms. For example, synchronization techniques
can be added or reduced-search motion compensation can be performed.
}

	hinclude { "GrayImage.h", "MVImage.h", "Error.h" }

//////// I/O AND STATES.
	input {		name { input }		type { packet } }
	input {		name { past }		type { packet } }
	output {	name { diffOut }	type { packet } }
	output {	name { mvOut }		type { packet } }

	defstate {
		name	{ BlockSize }
		type	{ int }
		default { 8 }
		desc	{ Size of blocks on which to do motion comp. }
	}

	protected { int blocksize; }
	start { blocksize = int(BlockSize); }

	virtual method {
		name { doSyncImage }
		access { protected }
		arglist { "(const GrayImage* inp)" }
		type { "void" }
		code {
			LOG_NEW; MVImage* mv = new MVImage;
			Packet pkt(*inp), empty(*mv);
			diffOut%0 << pkt;
			mvOut%0 << empty;
	}	}

	method {
		name { doMC }
		type { "void" }
		access { private }
		arglist { "(unsigned char* diff, unsigned const char* cur, \
				unsigned const char* prev, char* horz, char* vert, \
				const int width, const int height)" }
		code {
			int ii, jj, xvec, yvec;

			// Do top row.
			for(jj = 0; jj < width; jj += blocksize) {
				DoOneBlock(*horz, *vert, diff, cur, prev, 0, jj, 0, 0,
						width);
				horz++; vert++;
			}

			// Do middle rows.
			for(ii = blocksize; ii < height-blocksize; ii+=blocksize) {
				DoOneBlock(*horz, *vert, diff, cur, prev, ii, 0, 0, 0,
						width);
				horz++; vert++;

				for(jj = blocksize; jj < width-blocksize;
						jj += blocksize) {
					FindMatch(cur, prev, ii, jj, xvec, yvec, width);
					DoOneBlock(*horz, *vert, diff, cur, prev, ii, jj,
							xvec, yvec, width);
					horz++; vert++;
				}

				DoOneBlock(*horz, *vert, diff, cur, prev, ii,
						width-blocksize, 0, 0, width);
				horz++; vert++;
			} // end middle rows

			// Do bottom row.
			for(jj = 0; jj < width; jj += blocksize) {
				DoOneBlock(*horz, *vert, diff, cur, prev,
						height-blocksize, jj, 0, 0, width);
				horz++; vert++;
			}
		} // end code{}
	} // end doMC()


	virtual method {
		name { FindMatch }
		type { "void" }
		arglist { "(unsigned const char* cur, \
				unsigned const char* prev, const int ii, const int jj, \
				int& xvec, int& yvec, const int width)" }
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
		arglist { "(char& horz, char& vert, unsigned char* diff, \
				unsigned const char* cur, unsigned const char* prev, \
				const int ii, const int jj, const int xvec, \
				const int yvec, const int width)" }
		code { // Set diff frame and mvects.
			int i, j, tmp1, tmp2;
			for(i = 0; i < blocksize; i++) {
				tmp1 = (ii+i+yvec)*width + jj+xvec;
				tmp2 = (ii+i)*width + jj;
				for(j = 0; j < blocksize; j++) {
					diff[tmp2+j] = quant(cur[tmp2+j], prev[tmp1+j]);
			}	}
// NOTE THE -1's!! These are so the motion vector points FROM the past
// block TO the current block, rather than the other way around!
			horz = char(-1*xvec); vert = char(-1*yvec);

		}
	} // end DoOneBlock{}

// DIVIDE DIFFERENCE BY 2 TO FIT INTO 8 BITS!!
// ADD 128 SO NEGATIVE NUMBERS DON'T HAVE TO WRAP AROUND!!
	inline virtual method {
		name { quant }
		access { protected }
		type { "unsigned char" }
		arglist { "(const int i1, const int i2)"}
		code {
			int f = (i1 - i2) / 2;
			return ((unsigned char) (f + 128));
	}	}

	method {
		name	{ inputsOk }
		access	{ private }
		type	{ "int" }
		arglist	{ "(const GrayImage& one, const GrayImage& two)" }
		code {
			const int w = one.retWidth();
			const int h = one.retHeight();
			int retval = (w == two.retWidth());
			retval &= (h == two.retHeight());
			retval &= (w == (blocksize * (w / blocksize)));
			retval &= (h == (blocksize * (h / blocksize)));
			retval &= !one.fragmented();
			retval &= !one.processed();
			retval &= !two.fragmented();
			retval &= !two.processed();
			return(retval);
		}
	} // end inputsOk()

	go {
// Read inputs.
		Packet pastPkt, curPkt;
		(past%0).getPacket(pastPkt);
		(input%0).getPacket(curPkt);
		TYPE_CHECK(curPkt, "GrayImage");
		const GrayImage* inImage =
				(const GrayImage*) curPkt.myData();

// Initialize if this is the first input image.
		if (!pastPkt.typeCheck("GrayImage")) {
			doSyncImage(inImage);
			return;
		}

		const GrayImage* prvImage = (const GrayImage*) pastPkt.myData();
		if (!inputsOk(*inImage, *prvImage)) {
			Error::abortRun(*this, "Problem with input images.");
			return;
		}

// If we reached here, we already have read one image.
// Create difference and mvect images.
// clone() is overloaded, so if inImage is of a type DERIVED from
// GrayImage, outImage will be of the same derived type.
// Use clone(int) rather than clone() so we don't copy image data.
		GrayImage* outImage = (GrayImage*) inImage->clone(1);
		LOG_NEW; MVImage* mvImage = new MVImage(*outImage, blocksize);

////// Do the motion compensation.
		doMC(outImage->retData(), inImage->constData(),
				prvImage->constData(), mvImage->retHorz(),
				mvImage->retVert(), outImage->retWidth(),
				outImage->retHeight());

// Send the outputs on their way.
		Packet diffPkt(*outImage); diffOut%0 << diffPkt;
		Packet mvPkt(*mvImage); mvOut%0 << mvPkt;
	} // end go{}
} // end defstar { MotionCmp }

defstar {
	name		{ DctCodeInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 Regents of the University of California }
	location	{ SDF image palette }
	desc {
This star reads two coded DCTImages (one high priority and one
low-priority), inverts the run-length encoding,
and outputs the resulting DCTImage.
Protection is built in to avoid crashing even if some of the coded
input data is affected by loss.

NOTE!! This star is different from the SDFRunLenInv star.
This one works on DCTImages, not GrayImages.
	}
	seealso { DctCode }

	input	{ name	{ hiport }	type { packet } }
	input	{ name	{ loport }	type { packet } }
	output	{ name	{ output }	type { packet } }

	defstate {
		name	{ HiPri }
		type	{ int }
		default	{ 1 }
		desc	{ Number of DCT coeffs per block to 'highport' input. }
	}

//// CODE
	hinclude { "DCTImage.h", "Error.h" }

	protected {
// Numbers unlikely to come from a DCT...
		const float StartOfBlock = 524288.0;
		const float StartOfRun = 1048576.0;
	}


	method { // Do the run-length decoding.
		name { invRunLen }
		type { "void" }
		access { protected }
		arglist { "(DCTImage* hiImage, const DCTImage* loImage)" }
		code {
// Initialize.
			const int bSize = hiImage->retBS();
			const int width = hiImage->fullWidth();
			const int height = hiImage->fullHeight();
			const int fullFrame = width * height;

			if ((bSize != loImage->retBS()) ||
					(width != loImage->fullWidth()) ||
					(height != loImage->fullHeight())) {
				Error::abortRun(*this, "Two input images don't match");
				return;
			}

// Do DC image first.
			int size = hiImage->retFullSize();
			if ((size != hiImage->retSize()) || (size !=
					HiPri*fullFrame/(bSize*bSize))) { // i.e. fragmented
				Error::abortRun(*this, "DC image fragmented!");
				return;
			}
			int i, j, k, blk, tmp;
			const float* inPtr = hiImage->constData();
			LOG_NEW; float* outPtr = new float[fullFrame];
			for(k = 0; k < fullFrame; k++) { outPtr[k] = 0.0; }

			i = 0;
			for(j = 0; j < fullFrame; j += bSize * bSize) {
				for(k = 0; k < HiPri; k++) {
					outPtr[j + k] = inPtr[i++];
			}	}

// While still low priority input data left...
			size = loImage->retFullSize();
			if (size != loImage->retSize()) { // i.e. fragmented
				Error::abortRun(*this, "Low-pri image fragmented!");
				return;
			}
			inPtr = loImage->constData();

			i = 0;
			while (i < size) {

// Process each block, which starts with "StartOfBlock".
				while ((i < size) && (inPtr[i] != StartOfBlock)) {
					i++;
				}
				if (i < size-2) {
					i++;
					blk = int(inPtr[i++]);
					blk *= bSize*bSize;
					if ((blk >= 0) && (blk < fullFrame)) {
						blk += HiPri;
						k = 0;
						while ((i < size) && (k < bSize*bSize - HiPri)
								&& (inPtr[i] != StartOfBlock)) {
							if (inPtr[i] == StartOfRun) {
								i++;
								if (i < size) {
									int runLen = int(inPtr[i++]);
									for(int l = 0; l < runLen; l++ ) {
										outPtr[blk+k] = 0.0;
										k++;
								}	}
							} else {
								outPtr[blk+k] = inPtr[i++];
								k++;
						}	}
					} // if (blk OK)
				}
			} // end while (indx < size)

// Copy the data back.
			hiImage->setSize(fullFrame);
			float* tmpPtr = hiImage->retData();
			for(i = 0; i < fullFrame; i++) {
				tmpPtr[i] = outPtr[i];
			}

			LOG_DEL; delete outPtr;
		} // end { invRunLen }
	}


	go {
// Read input image.
		Packet hiPacket;
		(hiport%0).getPacket(hiPacket);
		TYPE_CHECK(hiPacket, "DCTImage");
		DCTImage* hiImage = (DCTImage*) hiPacket.writableCopy();

		Packet loPacket;
		(loport%0).getPacket(loPacket);
		TYPE_CHECK(loPacket, "DCTImage");
		const DCTImage* loImage = (const DCTImage*) loPacket.myData();

// Do the conversion.
		invRunLen(hiImage, loImage);

// Send output.
		Packet temp(*hiImage);
		output%0 << temp;
	}
} // end defstar{ DctCodeInv }

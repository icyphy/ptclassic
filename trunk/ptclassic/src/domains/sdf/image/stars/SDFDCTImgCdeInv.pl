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

	input	{ name	{ hiport }	type { message } }
	input	{ name	{ loport }	type { message } }
	output	{ name	{ output }	type { message } }

	defstate {
		name	{ HiPri }
		type	{ int }
		default	{ 1 }
		desc	{ Number of DCT coeffs per block to 'highport' input. }
	}

//// CODE
	hinclude { "DCTImage.h", "Error.h" }

	code {
// Numbers unlikely to come from a DCT...
		const float StartOfBlock = 524288.0;
		const float StartOfRun = 1048576.0;
	}


	method { // Do the run-length decoding.
		name { invRunLen }
		type { "void" }
		access { protected }
		arglist { "(DCTImage& hiImage, const DCTImage& loImage)" }
		code {
// Initialize.
			const int bSize = hiImage.retBS();
			const int fullFrame = hiImage.fullWidth() *
					hiImage.fullHeight();

// Do DC image first.
			int i, j, k, blk;
			const float* inPtr = hiImage.constData();
			LOG_NEW; float* outPtr = new float[fullFrame];
			for(k = 0; k < fullFrame; k++) { outPtr[k] = 0.0; }

			i = 0;
			for(j = 0; j < fullFrame; j += bSize * bSize) {
				for(k = 0; k < HiPri; k++) {
					outPtr[j + k] = inPtr[i++];
			}	}

// While still low priority input data left...
			const int size = loImage.retFullSize();
			inPtr = loImage.constData();

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
			hiImage.setSize(fullFrame);
			copy(fullFrame, hiImage.retData(), outPtr);
			LOG_DEL; delete [] outPtr;
		}
	} // end { invRunLen }


	method {
		name { copy }
		type { "void" }
		access { private }
		arglist { "(const int c, float* to, const float* from)" }
		code {
			for(int i = 0; i < c % 5; i++) {
				*to++ = *from++;
			}
			for(; i < c; i += 5) {
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
			}
	}	}


	go {
// Read input images.
		Envelope hiEnvp;
		(hiport%0).getMessage(hiEnvp);
		TYPE_CHECK(hiEnvp, "DCTImage");
		DCTImage* hiImage = (DCTImage*) hiEnvp.writableCopy();

		Envelope loEnvp;
		(loport%0).getMessage(loEnvp);
		TYPE_CHECK(loEnvp, "DCTImage");
		const DCTImage* loImage = (const DCTImage*) loEnvp.myData();

// Check some things.
		if ((hiImage->retBS() != loImage->retBS()) ||
				(hiImage->fullWidth() != loImage->fullWidth()) ||
				(hiImage->fullHeight() != loImage->fullHeight())) {
			LOG_DEL; delete hiImage;
			Error::abortRun(*this, "Two input images don't match");
			return;
		}
		if (hiImage->fragmented() || (hiImage->retFullSize() !=
				int(HiPri) * (hiImage->fullWidth() * hiImage->fullHeight()) /
				(hiImage->retBS() * hiImage->retBS()))) {
			LOG_DEL; delete hiImage;
			Error::abortRun(*this, "Hi-pri image wrong size.");
			return;
		}
		if (loImage->fragmented()) {
			LOG_DEL; delete hiImage;
			Error::abortRun(*this, "Low-pri image fragmented.");
			return;
		}

// Do the conversion.
		invRunLen(*hiImage, *loImage);

// Send output.
		Envelope temp(*hiImage);
		output%0 << temp;
	}
} // end defstar{ DctCodeInv }

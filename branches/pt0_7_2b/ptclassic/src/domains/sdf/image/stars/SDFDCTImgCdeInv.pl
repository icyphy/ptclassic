defstar {
	name		{ DctCodeInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 Regents of the University of California }
	location	{ ~haskell/Ptolemy }
	desc {
This star reads two coded DCTImages (one high priority and one
low-priority), inverts the run-length encoding and zig-zag scanning,
and outputs a correctly ordered DCTImage.
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

	method { // invert zig-zag scan. "imData" holds output.
		name { ziginv }
		type { "void" }
		access { private }
		arglist { "(float* imData, const float* fData, const int i, \
				const int j, const int width, const int blockSize)" }
		code {
			int k, l, indx;

// Invert the zigzag.
			indx = 0;
// K is length of current (semi)diagonal; L is iteration on diag.
			for(k = 1; k < blockSize; k++) { // Top triangle
				for(l = 0; l < k; l++) { // down
					imData[j + (i+l)*width + (k-l-1)] = fData[indx++];
				}
				k++; // NOTE THIS!
				for(l = 0; l < k; l++) { // back up
					imData[j + (i+k-l-1)*width + l] = fData[indx++];
			}	}

// If blockSize an odd number, start with diagonal, else one down.
			if (blockSize % 2) { k = blockSize; }
			else { k = blockSize-1; }

			for(; k > 1; k--) { // Bottom triangle
				for(l = 0; l < k; l++) { // down
					imData[j + (i+blockSize-k+l)*width +
							(blockSize-l-1)] = fData[indx++];
				}
				k--; // NOTE THIS!
				for(l = 0; l < k; l++) { // back up
					imData[j + (i+blockSize-l-1)*width +
							blockSize-k+l] = fData[indx++];
			}	}

// Have to do last element.
			imData[j + (i + blockSize - 1) * width + blockSize - 1]
					= fData[indx];
		} // end code {}
	} // end method { ziginv }


	method { // Do the run-length decoding.
		name { invRunLen }
		type { "void" }
		access { protected }
		arglist { "(DCTImage* hiImage, const DCTImage* loImage)" }
		code {
// Initialize.
			int bSize = hiImage->retBS();
			int width = hiImage->fullWidth();
			int height = hiImage->fullHeight();
			int fullFrame = width * height;

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
			int i, j, k, row, col, tmp;
			const float* inPtr = hiImage->constData();
			LOG_NEW; float* outPtr = new float[fullFrame];
			for(k = 0; k < fullFrame; k++) { outPtr[k] = 0.0; }

			i = 0;
			for(row = 0; row < height; row += bSize) {
				for(col = 0; col < width; col += bSize) {
					tmp = row*width + col*bSize;
					for(k = 0; k < HiPri; k++) {
						outPtr[tmp + k] = inPtr[i++];
			}	}	}

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
				if (i < size-3) {
					i++;
					row = int(inPtr[i++]);
					col = int(inPtr[i++]);
					if ((row % bSize == 0) && (col % bSize == 0) &&
							(row >= 0) && (col >= 0) &&
							(row < height) && (col < width)) {
						j = row*width + col*bSize + HiPri;
						k = 0;
						while ((i < size) && (k < bSize*bSize - HiPri)
								&& (inPtr[i] != StartOfBlock)) {
							if (inPtr[i] == StartOfRun) {
								i++;
								if (i < size) {
									int runLen = int(inPtr[i++]);
									for(int l = 0; l < runLen; l++ ) {
										outPtr[j+k] = 0.0;
										k++;
								}	}
							} else {
								outPtr[j+k] = inPtr[i++];
								k++;
						}	}
					} // if (row, column OK)
				}
			} // end while (indx < size)

// Copy the data back.
			hiImage->setSize(fullFrame);
			for(row = 0; row < height; row += bSize) {
				for(col = 0; col < width; col += bSize) {
					ziginv(hiImage->retData(),
							outPtr + row*width + col*bSize, row, col,
							width, bSize);
			}	}

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
	} // end go{}
} // end defstar{ DctCodeInv }

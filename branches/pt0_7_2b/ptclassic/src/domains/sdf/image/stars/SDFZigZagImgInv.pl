defstar {
	name		{ ZigZagInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 Regents of the University of California }
	location	{ SDF image palette }
	desc {
This star inverse zig-zag scans a DCTImage.
}
	seealso { ZigZag }

	input	{ name	{ inport }	type { packet } }
	output	{ name	{ outport }	type { packet } }

	hinclude { "DCTImage.h", "Error.h" }


	method {
		name { checkSize }
		type { int }
		access { protected }
		arglist { "(const DCTImage& img)" }
		code {
			int retval = (img.retFullSize() ==
					(img.fullWidth() * img.fullHeight()));
			retval &= (img.retFullSize() == img.retSize());
			return retval;
	}	}


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
		name { invZigZag }
		type { "void" }
		access { protected }
		arglist { "(DCTImage& img)" }
		code {
// Initialize.
			int bSize = img.retBS();
			int width = img.fullWidth();
			int height = img.fullHeight();

// Allocate space and go.
			int row, col;
			float* outArr = new float[img.retFullSize()];
			float* tmpPtr = img.retData();
			for(row = 0; row < height; row += bSize) {
				for(col = 0; col < width; col += bSize) {
					ziginv(outArr, tmpPtr, row, col, width,
							bSize);
					tmpPtr += bSize*bSize;
			}	}

// Copy the data back.
			copy(img.retFullSize(), img.retData(), outArr);
			LOG_DEL; delete outArr;
		}
	} // end { invZigZag }


	method {
		name { copy }
		type { void }
		arglist { "(const int c, float* to, const float* from)" }
		access { private }
		code {
			for(int i = 0; i < c % 5; i++) { *to++ = *from++; }

			for(i = c % 5; i < c; i += 5) {
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
			}
	}	}


	go {
		Packet inPkt;
		(inport%0).getPacket(inPkt);
		TYPE_CHECK(inPkt, "DCTImage");
		DCTImage* image = (DCTImage*) inPkt.writableCopy();

		if(!checkSize(*image)) {
			Error::abortRun(*this, "Processed or fragmented.");
			return;
		}

		invZigZag(*image);

		Packet temp(*image);
		outport%0 << temp;
	}
} // end defstar{ ZigZagInv }

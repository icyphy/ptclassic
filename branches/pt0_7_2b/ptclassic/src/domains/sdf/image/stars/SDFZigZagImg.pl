defstar {
	name		{ ZigZag }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ SDF image palette }
	desc {
This star zig-zag scans a DCTImage and outputs the result.
This is useful before quantization.
	}

	input	{ name { inport }	type { packet } }
	output	{ name { outport }	type { packet } }

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


	method { // zig-zag scan one block. "fData" holds output.
		name { zigzag }
		type { "void" }
		access { protected }
		arglist { "(float* fData, const float* imData, const int i, \
				const int j, const int width, const int blockSize)" }
		code {
			int k, l, indx;

// Do zig-zag scan.
			indx = 0;
// K is length of current (semi)diagonal; L is iteration along diag.
			for(k = 1; k < blockSize; k++) { // Top triangle
				for(l = 0; l < k; l++) { // down
					fData[indx++] = imData[j + (i+l)*width + (k-l-1)];
				}
				k++; // NOTE THIS!
				for(l = 0; l < k; l++) { // back up
					fData[indx++] = imData[j + (i+k-l-1)*width + l];
			}	}

// If blockSize an odd number, start with diagonal, else one down.
			if (blockSize % 2) { k = blockSize; }
			else { k = blockSize-1; }

			for(; k > 1; k--) { // Bottom triangle
				for(l = 0; l < k; l++) { // down
					fData[indx++] = imData[j + (i+blockSize-k+l)*width +
							(blockSize-l-1)];
				}
				k--; // NOTE THIS!
				for(l = 0; l < k; l++) { // back up
					fData[indx++] = imData[j + (i+blockSize-l-1)*width +
							blockSize-k+l];
			}	}

// Have to do last element.
			fData[indx] = imData[j + (i + blockSize - 1) * width +
					blockSize - 1];
		} // end code {}
	} // end zigzag {}


	method { // Do the run-length coding.
		name { doZigZag }
		type { "void" }
		access { protected }
		arglist { "(DCTImage& img)" }
		code {
// Initialize.
			const int bSize = img.retBS();
			const int width = img.fullWidth();
			const int height = img.fullHeight();

			LOG_NEW; float* outArr = new float[img.retFullSize()];

// For each row and col...
			int row, col;
			for(row = 0; row < height; row += bSize) {
				for(col = 0; col < width; col += bSize) {
					zigzag(outArr, img.retData(), row, col, width,
							bSize);
					outArr += bSize*bSize;
			}	} // end for(each row and column)

// Copy the data to the DCTImage.
			copy(img.retFullSize(), img.retData(), outArr);
			LOG_DEL; delete outArr;
		}
	} // end { doZigZag }


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
		if (!checkSize(*image)) {
			Error::abortRun(*this, "Processed or fragmented.");
			return;
		}
		doZigZag(*image);

		Packet outPkt(*image); outport%0 << outPkt;
	}
} // end defstar { ZigZag }
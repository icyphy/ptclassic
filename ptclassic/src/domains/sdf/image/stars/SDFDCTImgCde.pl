defstar {
	name		{ DctCode }
	domain		{ SDF }
	version		{ $Id$ }1
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ SDF image palette }
	desc {
This star takes a DCTImage input, chops it into blocks, zig-zag scans
it, inserts 'StartOfBlock' markers, run-length encodes it using
'StartOfRun' markers, and outputs a modified DCTImage.

For the run-length encoding, all values with absolute value less than
'Thresh' are set to 0.0, to help improve compression.

Runlengths are coded with a start symbol of 'StartOfRun' (defined below)
and then an (integer) run-length.

'HiPri' DCT coefficients per block are sent to 'hiport', the
high-priority output. The remainder of the coefficients are sent
to 'loport', the low-priority output.

NOTE!! This differs from the SDFRunLen star in that this block
processes DCTImages, not GrayImages.
	}

	input	{	name { inport }	type { packet } }
	output	{	name { hiport }	type { packet } }
	output	{	name { loport }	type { packet } }
	output	{	name { compr }	type { float } }

	defstate {
		name	{ Thresh }
		type	{ float }
		default { 0 }
		desc	{ Threshold for run-length coding. }
	}

	defstate {
		name	{ HiPri }
		type	{ int }
		default { 1 }
		desc { Number of DCT coefficients per block sent to 'hiport'. }
	}

// CODE
	hinclude { "DCTImage.h", "Error.h" }

	protected {
		const float StartOfBlock = 524288.0;
		const float StartOfRun = 1048576.0;
		float thresh;
	}

	start { thresh = float(fabs(double(Thresh))); }

	method { // zig-zag scan one block. "fData" holds output.
		name { zigzag }
		type { "void" }
		access { private }
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
		name { doRunLen }
		type { "void" }
		access { protected }
		arglist { "(DCTImage* dcImage, DCTImage* acImage)" }
		code {
// Initialize.
			int bSize = dcImage->retBS();
			int width = dcImage->fullWidth();
			int height = dcImage->fullHeight();
			int size = width * height;
			if (size != dcImage->retSize()) {
				Error::abortRun(*this,
						"Can't encode a fragment or precoded image!");
				return;
			}

// Temporary storage for one block.
			LOG_NEW; float* tmpBlk = new float[bSize*bSize];

// The biggest runlen blowup we can have is the string "01010101...".
// This gives a blowup of 50%, so with StartOfBlock and StartOfRun
// markers, 1.70 should be ok.
			LOG_NEW; float* outDc = new float[int(1.70*size + 1)];
			LOG_NEW; float* outAc = new float[int(1.70*size + 1)];

			int indxDc = 0, indxAc = 0, i, row, col, zeroRunLen;
			for(row = 0; row < height; row += bSize) {
				for(col = 0; col < width; col += bSize) {
					zigzag(tmpBlk, dcImage->retData(), row, col, width,
							bSize);

// High priority coefficients.
					for(i = 0; (i < bSize*bSize) && (i < HiPri); i++) {
						outDc[indxDc++] = tmpBlk[i];
					}

// Low priority coefficients--start with block header.
					outAc[indxAc++] = StartOfBlock;
					outAc[indxAc++] = float(row);
					outAc[indxAc++] = float(col);

					zeroRunLen = 0;
					for(; i < bSize*bSize; i++) {
// Do a non-zero run.
						if(zeroRunLen) {
							if (larger(tmpBlk[i], thresh)) {
								outAc[indxAc++] = float(zeroRunLen);
								zeroRunLen = 0;
								outAc[indxAc++] = tmpBlk[i];
							} else {
								zeroRunLen++;
							}
						} else {
							if (larger(tmpBlk[i], thresh)) {
								outAc[indxAc++] = tmpBlk[i];
							} else {
								outAc[indxAc++] = StartOfRun;
								zeroRunLen++;
					}	}	}
// Handle zero-runs that last till end of the block.
					if(zeroRunLen) {
						outAc[indxAc++] = float(zeroRunLen);
					}
			}	} // end for(each row and column)

// Copy the data to the DCTImages.
			LOG_DEL; delete tmpBlk;

			dcImage->setSize(indxDc);
			tmpBlk = dcImage->retData();
			for(i = 0; i < indxDc; i++) {
				tmpBlk[i] = outDc[i];
			}
			LOG_DEL; delete outDc;

			acImage->setSize(indxAc);
			tmpBlk = acImage->retData();
			for(i = 0; i < indxAc; i++) {
				tmpBlk[i] = outAc[i];
			}
			LOG_DEL; delete outAc;
		} // end { doRunLen }
	}


	inline method {
		name { larger }
		type { "int" }
		access { protected }
		arglist { "(const float fl, const float in)" }
		code { return (fabs(double(fl)) >= double(in)); }
	}


	go {
// Read input.
		Packet inPkt;
		(inport%0).getPacket(inPkt);
		TYPE_CHECK(inPkt, "DCTImage");

// Do processing and send out.
		DCTImage* dcImage = (DCTImage*) inPkt.writableCopy();
		DCTImage* acImage = (DCTImage*) dcImage->clone(1);
		doRunLen(dcImage, acImage);

		float comprRatio = dcImage->retFullSize() +
				acImage->retFullSize();
		comprRatio /= dcImage->fullWidth();
		comprRatio /= dcImage->fullHeight();
		compr%0 << comprRatio;

		Packet dcPkt(*dcImage); hiport%0 << dcPkt;
		Packet acPkt(*acImage); loport%0 << acPkt;
	} // end go{}
} // end defstar { DctCode }

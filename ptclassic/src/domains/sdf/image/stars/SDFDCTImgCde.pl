defstar {
	name		{ DctCode }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ SDF image palette }
	desc {
This star takes a DCTImage input, inserts 'StartOfBlock' markers,
run-length encodes it using 'StartOfRun' markers, and outputs the
modified DCTImage.

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

	method { // Do the run-length coding.
		name { doRunLen }
		type { "void" }
		access { protected }
		arglist { "(DCTImage* dcImage, DCTImage* acImage)" }
		code {
// Initialize.
			const int bSize = dcImage->retBS();
			const int width = dcImage->fullWidth();
			const int height = dcImage->fullHeight();
			const int size = width * height;
			const int blocks = size / (bSize*bSize);
			if (size != dcImage->retSize()) {
				Error::abortRun(*this,
						"Can't encode a fragment or precoded image!");
				return;
			}

// Temporary storage for one block.
			float* tmpPtr = dcImage->retData();

// The biggest runlen blowup we can have is the string "01010101...".
// This gives a blowup of 50%, so with StartOfBlock and StartOfRun
// markers, 1.70 should be ok.
			LOG_NEW; float* outDc = new float[int(1.70*size + 1)];
			LOG_NEW; float* outAc = new float[int(1.70*size + 1)];

			int indxDc = 0, indxAc = 0, i, blk, zeroRunLen;
			for(blk = 0; blk < blocks; blk++) {
// High priority coefficients.
				for(i = 0; i < HiPri; i++) {
					outDc[indxDc++] = *tmpPtr++;
				}

// Low priority coefficients--start with block header.
				outAc[indxAc++] = StartOfBlock;
				outAc[indxAc++] = float(blk);

				zeroRunLen = 0;
				for(; i < bSize*bSize; i++) {
					if(zeroRunLen) {
						if (larger(*tmpPtr, thresh)) {
							outAc[indxAc++] = float(zeroRunLen);
							zeroRunLen = 0;
							outAc[indxAc++] = *tmpPtr;
						} else {
							zeroRunLen++;
						}
					} else {
						if (larger(*tmpPtr, thresh)) {
							outAc[indxAc++] = *tmpPtr;
						} else {
							outAc[indxAc++] = StartOfRun;
							zeroRunLen++;
					}	}
					tmpPtr++;
				}
// Handle zero-runs that last till end of the block.
				if(zeroRunLen) {
					outAc[indxAc++] = float(zeroRunLen);
				}
			} // end for(each block)

// Copy the data to the DCTImages.
			dcImage->setSize(indxDc);
			tmpPtr = dcImage->retData();
			for(i = 0; i < indxDc; i++) {
				tmpPtr[i] = outDc[i];
			}
			LOG_DEL; delete outDc;

			acImage->setSize(indxAc);
			tmpPtr = acImage->retData();
			for(i = 0; i < indxAc; i++) {
				tmpPtr[i] = outAc[i];
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

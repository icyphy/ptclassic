defstar {
	name		{ RunLen }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Accept a GrayImage and run-length encode it. All values less than
'thresh' from 'meanVal' are set to 'meanVal' to help improve
compression.

Runlengths are coded with a start symbol of 'meanVal' and then a
run-length between 1 and 255. Runs longer than 255 must be coded in
separate pieces.
	}
	explanation {
.Id "compression, run length"
.Id "encoding, run length"
.Id "run length encoding"
.Id "image, run length encoding"
	}

	hinclude { "GrayImage.h", "Error.h" }

// INPUT AND STATES.
	input { name { input } type { packet } }
	output { name { outData } type { packet } }
	output { name { compression } type { float } }

	defstate {
		name	{ Thresh }
		type	{ int }
		default { 0 }
		desc	{ Threshold for run-length coding. }
	}
	defstate {
		name { meanVal }
		type { int }
		default { 0 }
		desc { Center value for thresholding. }
	}

// CODE.
	protected { int thresh; }
	start { thresh = int(Thresh); }

	inline method {
		name { gt }
		type { "int" }
		access { protected }
		arglist { "(unsigned char ch, int in)" }
		code {
			return (abs(int(ch) - int(meanVal)) > in);
		}
	}

	method {
		name { doRunLen }
		type { "void" }
		access { protected }
		arglist { "(GrayImage* inImage)" }
		code {
// Do the run-length coding.
			const int size = inImage->retFullSize();
			unsigned char* ptr1 = inImage->retData();

// The biggest blowup we can have is the string "01010101...".
// This gives a blowup of 33%, so 1.34 is ok.
			LOG_NEW;
			unsigned char* ptr2 = new unsigned char[int(1.34*size + 1)];

			int indx1 = 0, indx2 = 0;
			while (indx1 < size) {
				while ((indx1 < size) && (gt(ptr1[indx1], thresh))) {
					ptr2[indx2++] = ptr1[indx1++];
				}
				if (indx1 < size) {
					int thisRun = 1; indx1++;
					while ((indx1 < size) && (thisRun < 255) &&
							(!gt(ptr1[indx1], thresh))) {
						indx1++; thisRun++;
					}
					ptr2[indx2++] = (unsigned char) int(meanVal);
					ptr2[indx2++] = (unsigned char) thisRun;
				}
			}
// Copy the data back.
			inImage->setSize(indx2);
			ptr1 = inImage->retData();
			for(indx1 = 0; indx1 < indx2; indx1++) {
				ptr1[indx1] = ptr2[indx1];
			}
			LOG_DEL; delete ptr2;
		} // end { doRunLen }
	}

	go {
// Read input.
		Packet inPkt;
		(input%0).getPacket(inPkt);
		TYPE_CHECK(inPkt, "GrayImage");
		GrayImage* inImage = (GrayImage*) inPkt.writableCopy();
		if (inImage->fragmented() || inImage->processed()) {
			delete inImage;
			Error::abortRun(*this,
					"Need unfragmented and unprocessed input image.");
			return;
		}

// Do processing and send out.
		doRunLen(inImage);
		const float comprRatio = float(inImage->retFullSize()) /
				float(inImage->retWidth() * inImage->retHeight());
		compression%0 << comprRatio;

		Packet outPkt(*inImage);
		outData%0 << outPkt;
	}
} // end defstar { RunLen }

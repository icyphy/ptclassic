defstar {
	name		{ RunLenInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Accept a GrayImage and inverse run-length encode it.
Check to make sure we don't write past unallocated memory.
	}
	seealso { RunLen }

	hinclude { "GrayImage.h", "Error.h" }

// INPUT AND STATES.
	input {
		name { input }
		type { packet }
	}
	output {
		name { outData }
		type { packet }
	}
	defstate {
		name { meanVal }
		type { int }
		default { 0 }
		desc { Center value for thresholding. }
	}

// Note: we need to check for indx2 < fullFrame in case of lost data.
// Otherwise we might crash the program when we write past allocated
// memory.
	method {
		name { invRunLen }
		type { "void" }
		access { protected }
		arglist { "(GrayImage* inImage)" }
		code {
// Do the run-length coding.
			const int size = inImage->retSize();
			const int fullFrame = inImage->retWidth() *
					inImage->retHeight();
			unsigned char* ptr1 = inImage->retData();

			LOG_NEW; unsigned char* ptr2 = new unsigned char[fullFrame];

			int indx1 = 0, indx2 = 0, count;
			while ((indx1 < size) && (indx2 < fullFrame)) {
// Do a zero-run.
				while ((indx1 < size) && (ptr1[indx1] == int(meanVal))
						&& (indx2 < fullFrame)) {
					indx1++; // Skip to run length.
					for(count = 0; (count < int(ptr1[indx1])) &&
							(indx2 < fullFrame); count++) {
						ptr2[indx2++] = (unsigned char) int(meanVal);
					}
					indx1++; // Skip past run length.
				}
// Handle a non-zero run.
				while ((indx1 < size) && (ptr1[indx1] != int(meanVal))
						&& (indx2 < fullFrame)) {
					ptr2[indx2++] = ptr1[indx1++];
				}
			}

// Copy the data back.
			inImage->setSize(fullFrame);
			ptr1 = inImage->retData();
			for(indx1 = 0; indx1 < indx2; indx1++) {
				ptr1[indx1] = ptr2[indx1];
			}
			for(; indx1 < fullFrame; indx1++) {
				ptr1[indx1] = (unsigned char) 0;
			}
			LOG_DEL; delete ptr2;
		} // end { invRunLen }
	}

	go {
// Read input.
		Packet inPkt;
		(input%0).getPacket(inPkt);
		TYPE_CHECK(inPkt,"GrayImage");

// Do processing and send out.
		GrayImage* inImage = (GrayImage*) inPkt.writableCopy();
		if (inImage->fragmented()) {
			delete inImage;
			Error::abortRun(*this,
					"Need unfragmented input image.");
			return;
		}
		invRunLen(inImage);
		Packet outPkt(*inImage);
		outData%0 << outPkt;
	}
} // end defstar { RunLenInv }

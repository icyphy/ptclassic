defstar {
	name		{ RunLenInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Accept a GrayImage and inverse run-length encode it.
See SDFrunLen.
	}

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

// CODE.
	method {
		name { invRunLen }
		type { "void" }
		access { protected }
		arglist { "(GrayImage* inImage)" }
		code {
// Do the run-length coding.
			int size = inImage->retSize();
			unsigned char* ptr1 = inImage->retData();

			LOG_NEW; unsigned char* ptr2 = new unsigned char[
					int(inImage->retWidth() * inImage->retHeight())];

			int indx1 = 0, indx2 = 0, count;
			while (indx1 < size) {
				while ((indx1 < size) && (ptr1[indx1] == 0 )) {
					indx1++; // Skip to run length.
					for(count = 0; count < int(ptr1[indx1]); count++) {
						ptr2[indx2++] = (unsigned char) 0;
					}
					indx1++; // Skip past run length.
				}
				while ((indx1 < size) && (ptr1[indx1] != 0)) {
					ptr2[indx2] = ptr1[indx1]; indx2++; indx1++;
				}
			}
// Copy the data back.
			inImage->setSize(indx2);
			ptr1 = inImage->retData();
			for(indx1 = 0; indx1 < indx2; indx1++) {
				ptr1[indx1] = ptr2[indx1];
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

defstar {
	name		{ RunLen }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Accept a GrayImage and run-length encode it.  All values less than
'thresh' are set to 0, to help improve compression.

Runlengths are coded with a start symbol of '0' and then a run-length
between 1 and 255.  Runs longer than 255 must be coded in separate
pieces.
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
	defstate {
		name	{ Thresh }
		type	{ int }
		default { 0 }
		desc	{ Threshold for run-length coding. }
	}

// CODE.
	protected { int thresh; }
	start { thresh = int(Thresh); }

	method {
		name { gt }
		type { "int" }
		access { protected }
		arglist { "(unsigned char ch, int in)" }
		code {
			static char a;
			a = char(ch);
			return (abs(int(a)) > in);
		}
	}

	method {
		name { doRunLen }
		type { "void" }
		access { protected }
		arglist { "(GrayImage* inImage)" }
		code {
// Do the run-length coding.
			int size = inImage->retWidth() * inImage->retHeight();
			unsigned char* ptr1 = inImage->retData();

// The biggest blowup we can have is the string "01010101...".
// This gives a blowup of 33%, so 1.34 is ok.
			unsigned char* ptr2 = new unsigned char[int(1.34*size + 1)];

			int indx1 = 0, indx2 = 0;
			while (indx1 < size) {
				while ((indx1 < size) && (gt(ptr1[indx1], thresh))) {
					ptr2[indx2] = ptr1[indx1]; indx2++; indx1++;
				}
				if (indx1 < size) {
					int thisRun = 1; indx1++;
					while ((indx1 < size) && (thisRun < 255) &&
							(!gt(ptr1[indx1], thresh))) {
						indx1++; thisRun++;
					}
					ptr2[indx2++] = (unsigned char) 0;
					ptr2[indx2++] = (unsigned char) thisRun;
				}
			}
// Copy the data back.
			inImage->setSize(indx2);
			ptr1 = inImage->retData();
			for(indx1 = 0; indx1 < indx2; indx1++) {
				ptr1[indx1] = ptr2[indx1];
			}
			delete ptr2;
		} // end { doRunLen }
	}

	go {
// Read input.
		Packet inPkt;
		(input%0).getPacket(inPkt);
		if (!StrStr(inPkt.dataType(), "GrayI")) {
			Error::abortRun(*this, inPkt.typeError("GrayI"));
			return;
		}

// Do processing and send out.
		GrayImage* inImage = (GrayImage*) inPkt.writableCopy();
		doRunLen(inImage);
		Packet outPkt(*inImage);
		outData%0 << outPkt;
	} // end go{}
} // end defstar { RunLen }

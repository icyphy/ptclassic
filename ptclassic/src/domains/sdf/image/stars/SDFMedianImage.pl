defstar {
	name		{ MedianImage }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
Accept a black-and-white image from an input packet, median-filter the
image, and send the result to the output.

Filter widths of 1, 3, 5 work well.  Any length longer than five will
take a long time to run.

Median filtering is useful for removing impulse-type noise from images.
It also smooths out textures, so it is a useful pre-processing step
before edge detection.  It removes inter-field flicker quite well
when displaying single frames from a moving sequence.
	}

	ccinclude { "GrayImage.h", "UserOutput.h" }

// INPUT AND STATES.
	input {
		name { inData }
		type { packet }
	}
	output {
		name { outData }
		type { packet }
	}
	defstate {
		name	{ FilterWidth }
		type	{ int }
		default { 3 }
		desc	{ Size of median filter window (should be odd number). }
	}

////// CODE.

	code {
	// Function to sort unsigned char's.
	extern "C" {
	static int sortUC(unsigned char* a, unsigned char*b)
	{
		if (*a < *b) return -1;
		else if (*a == *b) return 0;
		else return 1;
	}
	}
	}

	protected {
		int width, height, size;
		unsigned char* buf;
	}

	start {
		size = int(FilterWidth);
		if (size <= 0) size = 3;
		size = 1 + 2*(size/2); // an odd number for sure
		buf = new unsigned char[size*size];
	}

	wrapup { delete buf; }

	method {
		name	{ retMedian }
		access	{ protected }
		type	{ "unsigned char" }
		arglist { "(unsigned char* p, int pi, int pj)" }

		code
		{
		// Copy data to buf.
			int i, j, cnt = 0;
			for(i = pi-size/2; i <= pi+size/2; i++) {
				for(j = pj-size/2; j <= pj+size/2; j++) {
					buf[cnt++] = p[i*width+j];
			}   }

		// Sort data and return result.
			qsort(buf, size*size, sizeof(unsigned char), sortUC);
			return(buf[(size/2) * (size+1)]); // median value.
		} // end code {}
	} // end method { retMedian }

	go {
// Read data from input and initialize.
		Packet inPkt;
		(inData%0).getPacket(inPkt);
		TYPE_CHECK(inPkt,"GrayImage");
		GrayImage* inImage = (GrayImage*) inPkt.myData();
		width = inImage->retWidth();
		height = inImage->retHeight();

// Create output image. clone(1) prevents memory copying.
		GrayImage* outImage = (GrayImage*) inImage->clone(1);

// Do the median filtering--don't do boundary pixels for now.
		int i, j;
		unsigned char* outP = outImage->retData();
		unsigned char* inP = inImage->retData();
		for(i = size/2; i < height-size/2; i++) {
			for(j = size/2; j < width-size/2; j++) {
				outP[j + i*width] = retMedian(inP, i, j);
		}   }

// Send the output on its way.
		Packet outPkt(*outImage);
		outData%0 << outPkt;
	} // end go{}

} // end defstar { MedianImage }

defstar {
	name		{ MedianImage }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept an input GrayImage, median filter the image, and send the result
to the output.  Filter widths of 1, 3, 5 work well.  Any length longer
than five will take a long time to run.

Median filtering is useful for removing impulsive noise from images.
It also smooths out textures, so it is a useful pre-processing step
before edge detection.  It removes inter-field flicker quite well
when displaying single frames from a moving sequence.
	}
	explanation {
Pixels at the image boundaries are copied and not median filtered.
.Id "filter, median"
.Id "image, median filter"
.Id "median filtering"
	}

	input { name { inData } type { message } }

	output { name { outData } type { message } }

	defstate {
		name	{ FilterWidth }
		type	{ int }
		default { 3 }
		desc	{ Size of median filter window (should be odd number). }
	}

	ccinclude { "GrayImage.h", <std.h> }

	code {
		// Function to sort unsigned char's.
		// These types and casts are required to satisfy cfront's
		// ridiculously strict rules.
		extern "C" {
			static int sortUC(const void* aV, const void* bV) {
				unsigned const char* a = (unsigned const char*) aV;
				unsigned const char* b = (unsigned const char*) bV;
				if (*a < *b) return -1;
				else if (*a == *b) return 0;
				else return 1;
	}	}	}

	protected {
		int width, height, size;
		unsigned char* buf;
	}

	constructor { buf = 0; }

	setup {
		size = int(FilterWidth);
		if (size <= 0) { size = 3; }
		size = 1 + 2*(size/2);		// an odd number for sure
		delete [] buf;
		buf = new unsigned char[size*size];
	}

	destructor { delete [] buf; }

	method {
		name	{ retMedian }
		access	{ protected }
		type	{ "unsigned char" }
		arglist { "(unsigned const char* p, int pi, int pj)" }
		code {
			// Copy data to buf.
			int cnt = 0;
			for(int i = pi-size/2; i <= pi+size/2; i++) {
				for(int j = pj-size/2; j <= pj+size/2; j++) {
					buf[cnt++] = p[i*width+j];
				}
			}

			// Sort data
			int numitems = size * size;
			qsort(buf, numitems, sizeof(unsigned char), sortUC);

			// Data indexed from 0 ... numitems - 1
			// Median value is at the middle index
			return buf[ (numitems - 1)/2 ];
		} // end code {}
	} // end method { retMedian }


	go {
		// Read data from input and initialize.
		Envelope inEnvp;
		(inData%0).getMessage(inEnvp);
		TYPE_CHECK(inEnvp, "GrayImage");
		const GrayImage* inImage = (const GrayImage*) inEnvp.myData();
		if (inImage->fragmented() || inImage->processed()) {
		    Error::abortRun(*this,
				    "Need a full-sized input image.");
		    return;
		}
		width = inImage->retWidth();
		height = inImage->retHeight();

		// Median filtering -- don't do boundary pixels for now
		GrayImage* outImage = (GrayImage*) inImage->clone(1);
		unsigned char* outP = outImage->retData();
		unsigned const char* inP = inImage->constData();
		int halfsize = size / 2;
		for (int i = halfsize; i < height; i++) {
		    int rowloc = i * width;
		    for (int j = 0; j < width; j++) {
			if ( (halfsize <= i) && (i < height - halfsize) &&
			     (halfsize <= i) && (j < width - halfsize) ) {
			    outP[j + rowloc] = retMedian(inP, i, j);
			}
			// Do not apply median definition at boundaries
			else {
			    outP[j + rowloc] = inP[j + rowloc];
			}
		    }
		}

		// Send the output on its way.
		Envelope outEnvp(*outImage);
		outData%0 << outEnvp;
	} // end go{}
} // end defstar { MedianImage }

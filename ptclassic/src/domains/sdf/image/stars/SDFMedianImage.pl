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
to the output.  Filter widths of 3, 5, and 7 work well.  Any length longer
than seven will take a long time to run.

Median filtering is useful for removing impulsive noise from images.
It also smooths out textures, so it is a useful pre-processing step
before edge detection.  It removes inter-field flicker quite well
when displaying single frames from a moving sequence.
	}
	explanation {
The \fIFilterWidth\fR parameter will always be odd.
If an even value is specified, the next higher odd number will be used.
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
		desc	{
Size of median filter window in each dimension (should be odd number). }
	}

	defstate {
		name	{ RankOrder }
		type	{ int }
		default { 0 }
		desc	{
Which ordered sample is chosen from each neighborhood of pixels,
0, 1, ... FilterWidth^2 - 1.
		}
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
		}
	    }
	}

	protected {
		int halfsize, height, neighborhoodSize, rankIndex, size, width;
		unsigned char* buf;
	}

	constructor {
		buf = 0;
		RankOrder.clearAttributes(A_SETTABLE);
	}

	setup {
		// Define the size of neighborhood in each dimension
		size = int(FilterWidth);
		if (size <= 0) { size = 3; }
		size = 1 + 2*(size/2);		// an odd number for sure
		FilterWidth = size;
		halfsize = size/2;

		// In a size x size neighborhood of pixels, data is
		// written into the one-dimensional buffer buf, which
		// is indexed from 0 ... (neighborhoodSize - 1).
		// Median value at middle index, (neighborhoodSize - 1)/2
		rankIndex = (size*size - 1)/2;
		RankOrder = rankIndex;

		// Define the size of the two-dimensional neighborhood
		neighborhoodSize = size * size;
		delete [] buf;
		buf = new unsigned char[neighborhoodSize];
	}

	destructor { delete [] buf; }

	method {
		name	{ retRankedElement }
		access	{ protected }
		type	{ "unsigned char" }
		arglist { "(unsigned const char* p, int pi, int pj)" }
		code {
		    // Select size x size neighbor around pixel (pi, pj)
		    // and write data to one-dimensional buffer
		    // We want to copy pixel values at p[i][j] where
		    //   i = (pi - halfsize) ... pi ... (pi + halfsize)
		    //   j = (pj - halfsize) ... pj ... (pj + halfsize)
		    int maxi = pi + halfsize;
		    int pjstart = pj - halfsize;
		    unsigned char *bufp = buf;
		    for (int i = pi - halfsize; i <= maxi; i++) {
			// pijptr begins at p[i][pj - halfsize]
			unsigned const char *pijptr = &p[i * width + pjstart];
			for (int k = 0; k < size; k++) {
			    *bufp++ = *pijptr++;
			}
		    }

		    // Sort data
		    qsort(buf, neighborhoodSize, sizeof(unsigned char), sortUC);

		    // Return ranked data elemented 
		    return buf[ rankIndex ];
		}
	}

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

		// Median filtering -- don't filter boundary pixels for now
		width = inImage->retWidth();
		height = inImage->retHeight();
		GrayImage* outImage = (GrayImage*) inImage->clone(1);
		unsigned char* outptr = outImage->retData();
		unsigned const char* inptr = inImage->constData();
		for (int i = 0; i < height; i++) {
		    int j = 0;
		    if ( (halfsize <= i) && (i < height - halfsize) ) {
			// Just copy the boundary pixels
			for ( ; j < halfsize; j++) {
			    *outptr++ = *inptr++;
			}
			// Median filter
			for ( ; j < width - halfsize; j++) {
			    *outptr++ = retRankedElement(inptr, i, j);
			    inptr++;
			}
			// Just copy the boundary pixels
			for ( ; j < width; j++) {
			    *outptr++ = *inptr++;
			}
		    }
		    // Just copy the boundary pixels
		    else {
			for ( ; j < width; j++) {
			    *outptr++ = *inptr++;
			}
		    }
		}

		// Send the output on its way.
		Envelope outEnvp(*outImage);
		outData%0 << outEnvp;
	} // end go{}
} // end defstar { MedianImage }

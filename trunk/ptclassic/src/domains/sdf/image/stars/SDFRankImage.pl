defstar {
	name		{ RankImage }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Brian Evans and Paul Haskell }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept an input GrayImage, rank filter the image, and send the result
to the output.  A common example of a rank filter is the median
filter, e.g. SDFMedianImage, which is derived from this star.
Pixels at the image boundaries are copied and not rank filtered.
	}
	explanation {
.Id "filter, rank"
.Id "image, rank filter"
.Id "rank filtering"
	}

	input { name { inData } type { message } }

	output { name { outData } type { message } }

	defstate {
		name	{ FilterWidth }
		type	{ int }
		default { 3 }
		desc	{
Size of the rank filter window in each dimension.
For a median filter, this value must be odd.
		}
	}

	defstate {
		name	{ RankOrder }
		type	{ int }
		default { 0 }
		desc	{
Which ordered sample is chosen from each neighborhood of pixels,
0, 1, ..., FilterWidth^2 - 1.
		}
	}

	ccinclude { <std.h>, "GrayImage.h" }

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
	}

	setup {
		size = int(FilterWidth);
		if (size <= 0) {
		    Error::error(*this, "FilterWidth must be positive.");
		    return;
		}
		halfsize = size/2;
		neighborhoodSize = size * size;

		rankIndex = int(RankOrder);
		if (rankIndex < 0) {
		    Error::error(*this, "RankOrder must be non-negative.");
		    return;
		}
		if (rankIndex >= neighborhoodSize) {
		    Error::error(*this, "RankOrder is too large.");
		    return;
		}

		delete [] buf;
		buf = new unsigned char[neighborhoodSize];

		width = height = 0;
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
		    unsigned const char *pijptr;
		    for (int i = pi - halfsize; i <= maxi; i++) {
			// pijptr begins at p[i][pj - halfsize]
			pijptr = &p[i * width + pjstart];
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
		    if ( (halfsize <= i) && (i < height - halfsize) ) {
		        int j = 0;
			for ( ; j < halfsize; j++) {	// copy boundary pixels
			    *outptr++ = *inptr++;
			}
			for ( ; j < width - halfsize; j++) {	// med. filter
			    *outptr++ = retRankedElement(inptr, i, j);
			    inptr++;
			}
			for ( ; j < width; j++) {	// copy boundary pixels
			    *outptr++ = *inptr++;
			}
		    }
		    else {
		        int j = 0;
			for ( ; j < width; j++) {	// copy boundary pixels
			    *outptr++ = *inptr++;
			}
		    }
		}

		// Send the output on its way.
		Envelope outEnvp(*outImage);
		outData%0 << outEnvp;
	} // end go{}
} // end defstar { RankImage }

defstar {
	name		{ RankImage }
	domain		{ SDF }
	version		{ @(#)SDFRankImage.pl	1.6 01 Oct 1996 }
	author		{ Brian Evans and Paul Haskell }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept an input grayimage represented by a float matrix, rank 
filter the image, and send the result to the output.  A common 
example of a rank filter is the median filter, e.g. SDFMedianImage, 
which is derived from this star. Pixels at the image boundaries 
are copied and not rank filtered.
	}
	htmldoc {
<a name="filter, rank"></a>
<a name="image, rank filter"></a>
<a name="rank filtering"></a>
	}

	input { name { inData } type { FLOAT_MATRIX_ENV } }

	output { name { outData } type { FLOAT_MATRIX_ENV } }

	defstate {
		name	{ FilterWidth }
		type	{ int }
		default { 3 }
		desc	{
Size of the rank filter window in each dimension,
which must be odd.
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

	hinclude { <std.h>, "Matrix.h" }

	code {
	    // Function to sort float's.
	    // These types and casts are required to satisfy cfront's
	    // ridiculously strict rules.
	    extern "C" {
		static int sortUC(const void* aV, const void* bV) {
		    float* a = (float*) aV;
		    float* b = (float*) bV;
		    if (*a < *b) return -1;
		    else if (*a == *b) return 0;
		    else return 1;
		}
	    }
	}

	protected {
		int halfsize, height, neighborhoodSize, rankIndex, size, width;
		float* buf;
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
		int oddsize = 1 + 2*(size/2);
		if ( size != oddsize ) {
		    Error::error(*this, "FilterWidth must be odd.");
		    return;
		}
		halfsize = (size - 1)/2;
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
		buf = (float *) new float[neighborhoodSize];

		width = height = 0;
	}

	destructor { delete [] buf; }

	method {
		name	{ retRankedElement }
		access	{ protected }
		type	{ "float" }
		arglist { "(const FloatMatrix& image, int i, int j)" }
		code {
		    // Select size x size neighbor around pixel at [i][j]
		    // and write 2-D data to 1-D buffer buf by copying
		    // pixel values at [i+k][j+l] where
		    //   k = -halfsize ... 0 ... halfsize
		    //   l = -halfsize ... 0 ... halfsize
		    float *bufptr = buf;
		    for (int k = -halfsize; k <= halfsize; k++)
			for (int l = -halfsize; l <= halfsize; l++)
			    *bufptr++ = image[i+k][j+l];

		    // Sort data
		    qsort(buf, neighborhoodSize, sizeof(float), sortUC);

		    // Return ranked data elemented 
		    return buf[ rankIndex ];
		}
	}

	go {
		// Read data from input.
                Envelope pkt;
                (inData%0).getMessage(pkt);
                const FloatMatrix& inImage = *(const FloatMatrix*)pkt.myData();

		width = inImage.numCols();
		height = inImage.numRows();

		// Median filtering -- don't filter boundary pixels for now
		LOG_NEW;
		FloatMatrix& outImage = *(new FloatMatrix(height,width));

		for (int i = 0; i < height; i++) {
		    if ( (halfsize <= i) && (i < height - halfsize) ) {
		        int j = 0;
			for ( ; j < halfsize; j++) {	// copy boundary pixels
			    outImage[i][j] = inImage[i][j];
			}
			for ( ; j < width - halfsize; j++) {	// med. filter
			    outImage[i][j] = retRankedElement(inImage,i,j);
			}
			for ( ; j < width; j++) {	// copy boundary pixels
			    outImage[i][j] = inImage[i][j];
			}
		    }
		    else {
		        int j = 0;
			for ( ; j < width; j++) {	// copy boundary pixels
			    outImage[i][j] = inImage[i][j];
			}
		    }
		}

		// Send the output on its way.
		outData%0 << outImage;
	} // end go{}
} // end defstar { RankImage }

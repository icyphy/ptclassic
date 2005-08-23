defstar {
	name		{ MedianImage }
	domain		{ SDF }
	derivedFrom	{ RankImage }
	version		{ @(#)SDFMedianImage.pl	1.29 06 Oct 1996 }
	author		{ Paul Haskell }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
	htmldoc {
The <i>FilterWidth</i> parameter must be an odd, positive integer.
Pixels at the image boundaries are copied and not median filtered.
<a name="filter, median"></a>
<a name="image, median filter"></a>
<a name="median filtering"></a>
	}

	constructor {
		RankOrder.clearAttributes(A_SETTABLE|A_CONSTANT);
	}

	setup {
		// In a fwidth x fwidth neighborhood of pixels, data is
		// written into the one-dimensional buffer buf, which
		// is indexed from 0 ... (fwidth * fwidth - 1).
		// Median value at middle index, (fwidth * fwidth - 1)/2
		int fwidth = int(FilterWidth);
		RankOrder = (fwidth*fwidth - 1)/2;

		// Initialize data members
		SDFRankImage::setup();
	}

} // end defstar { MedianImage }

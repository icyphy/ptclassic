defstar {
	name	{ DiffImage }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Paul Haskell }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept two black-and-white images from input GrayImages, take
their absolute difference, scale the absolute difference,
and pass the result to the output.
	}
	explanation {
.Id "image difference"
}

	ccinclude { "GrayImage.h", "Error.h" }

// INPUT AND STATES.
	input { name { input1 } type { message } }
	input { name { input2 } type { message } }
	output { name { outData } type { message } }
	defstate {
		name	{ Scale }
		type	{ float }
		default { 1.0 }
		desc	{ Amount by which to scale the image difference. }
	}

	go {
		// Read inputs.
		Envelope inEnvp1, inEnvp2;
		(input1%0).getMessage(inEnvp1);
		(input2%0).getMessage(inEnvp2);
		TYPE_CHECK(inEnvp1, "GrayImage");
		TYPE_CHECK(inEnvp2, "GrayImage");
		const GrayImage* img1 = (const GrayImage*) inEnvp1.myData();
		const GrayImage* img2 = (const GrayImage*) inEnvp2.myData();

		// Calculate the difference between the images
		int width = img1->retWidth();
		int height = img1->retHeight();

		if ((width != img2->retWidth()) ||
		    (height != img2->retHeight())) {
			Error::abortRun(*this, "Image sizes do not match.");
			return;
		}
		if (img1->fragmented() || img1->processed() ||
		    img2->fragmented() || img2->processed()) {
			Error::abortRun(*this,
					"Can't handle fragmented or processed images.");
			return;
		}

		// Allocate a copy of the first image to hold result
		GrayImage *img3 = new GrayImage(*img1);

		// Compute the pixel-by-pixel difference
		double scale = Scale;
		unsigned int maxpixel = width * height;
		unsigned const char* ptr2 = img2->constData();
		unsigned char* ptr3 = img3->retData();
		for (unsigned int pixel = 0; pixel < maxpixel; pixel++) {
			// take absolute value of scaled difference
			double diff = scale *
					( double(*ptr3) - double(*ptr2++) );
			if (diff < 0.0) diff = -diff;

			// map the intensity values into the range [0, 255]
			if (diff < 0.5)	*ptr3++ = (unsigned char) 0;
			else if (diff > 254.5) *ptr3++ = (unsigned char) 255;
			else *ptr3++ = (unsigned char) (diff + 0.5);
		}

		// Send the result.
		Envelope outEnvp(*img3);
		(outData%0) << outEnvp;
	} // end go{}
} // end defstar { DiffImage }

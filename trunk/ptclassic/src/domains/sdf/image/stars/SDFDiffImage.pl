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
		GrayImage* img1 = (GrayImage*) inEnvp1.writableCopy();
		const GrayImage* img2 = (const GrayImage*) inEnvp2.myData();

		// Calculate the difference between the images
		int width = img1->retWidth();
		int height = img1->retHeight();

		if ((width != img2->retWidth()) ||
		    (height != img2->retHeight())) {
			delete img1;
			Error::abortRun(*this, "Image sizes do not match.");
			return;
		}
		if (img1->fragmented() || img1->processed() ||
		    img2->fragmented() || img2->processed()) {
			delete img1;
			Error::abortRun(*this,
					"Can't handle fragmented or processed images.");
			return;
		}

		unsigned char* ptr1 = img1->retData();
		unsigned const char* ptr2 = img2->constData();
		double scale = Scale;
		unsigned int maxindex = width*height;
		for (unsigned int travel = 0; travel < maxindex; travel++) {
			// take absolute value of scaled difference
			double diff;
			diff = scale * ( double(*ptr1) - double(*ptr2) );
			if (diff < 0.0) diff = -diff;

			// map the intensity values into the range [0, 255]
			if (diff < 0.5)	*ptr1 = (unsigned char) 0;
			else if (diff > 254.5) *ptr1 = (unsigned char) 255;
			else *ptr1 = (unsigned char) (diff + 0.5);
			ptr1++;
			ptr2++;
		}

		// Send the result.
		Envelope outEnvp(*img1);
		(outData%0) << outEnvp;
	} // end go{}
} // end defstar { DiffImage }

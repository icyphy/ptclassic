defstar {
	name	{ SumImage }
	domain	{ SDF }
	version	{ $Id$ }
	author	{ Alan Kamas }
	acknowledge { based on the SDFDiffImage star by Paul Haskell }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
Accept black-and-white images from input GrayImages, take
their absolute sum, scale the sum, and pass the result to the output.
	}
	explanation {
.Id "image sum"
}
	ccinclude { "GrayImage.h", "Error.h" }

// INPUT AND STATES.
	input { 
		name { input1 } 
		type { message } 
	}
	input { 
		name { input2 } 
		type { message } 
	}
	output { 
		name { outData } 
		type { message } 
	}
	defstate {
		name	{ Scale }
		type	{ float }
		default { 0.5 }
		desc	{ Amount by which to scale the result }
	}

// CODE.
	go {
// Read inputs.
		Envelope inEnvp1, inEnvp2;
		(input1%0).getMessage(inEnvp1);
		(input2%0).getMessage(inEnvp2);
		TYPE_CHECK(inEnvp1, "GrayImage");
		TYPE_CHECK(inEnvp2, "GrayImage");
		GrayImage* img1 = (GrayImage*) inEnvp1.writableCopy();
		const GrayImage* img2 = (const GrayImage*) inEnvp2.myData();

// Calc the sum.
		int width, height = 0;
		if (((width = img1->retWidth()) != img2->retWidth()) ||
				((height = img1->retHeight()) != img2->retHeight())) {
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
		float sum;
		double scale = Scale;
		for(int travel = 0; travel < width*height; travel++) {
			sum = float(ptr1[travel]) + float(ptr2[travel]);
			sum = fabs(sum*scale);
			if (sum < 0.5)		ptr1[travel] = (unsigned char) 0;
			else if (sum > 254.5) ptr1[travel] = (unsigned char) 255;
			else ptr1[travel] = (unsigned char) (sum + 0.5);
		}

// Send the result.
		Envelope outEnvp(*img1);
		(outData%0) << outEnvp;
	} // end go{}
} // end defstar { DiffImage }

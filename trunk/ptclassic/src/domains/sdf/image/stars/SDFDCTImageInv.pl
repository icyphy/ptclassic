defstar {
	name		{ DCTImageInv }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Sun-Inn Shih, Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
This star takes a DCTImage input and does inverse
discrete cosine transform (DCT) coding and outputs a GrayImage.
	}
	explanation {
.Id "DCT inverse"
.Id "discrete cosine transform inverse"
.Id "image, discrete cosine transform inverse"
.Id "inverse discrete cosine transform"
.Id "inverse DCT"
}

	input { name { input } type { message } }
	output { name { output } type { message } }

// CODE
	hinclude { <math.h>, "GrayImage.h", "DCTImage.h", "Error.h" }

	protected {
		int firstTime;
		float* cosData;
		int blocksize;
	}

	method {
		name { cosSet } // SAME as fwd DCT BASIS matrix.
		type { void }
		access { protected }
		code {
			int i, j;
			double lambda = M_PI / (2.0 * blocksize);
			double t1 = 1.0 / sqrt(double(blocksize));
			double t2 = sqrt(2.0 / double(blocksize));
			for(j = 0; j < blocksize; j++) {
				cosData[j] = float(t1);
				for(i = 1; i < blocksize; i++) {
					cosData[j+blocksize*i] = float( t2 *
							cos(double((2*j+1)*i*lambda)) );
			}	}
		}
	} // end cosSet()

	inline method { // Inverse DCT basis values (note transpose).
		name { invD }
		type { float }
		arglist { "(int a, int b)" }
		access { protected }
		code { return (cosData[b*blocksize+a]); }
	} // end invD()

	method {
		name { doInvDCT }
		type { void }
		access { protected }
		arglist {
			"(unsigned char* outdata, float* indata, int outw,
				int outh, int inw, int inh)"
		}
		code {
	register int ndx, ndx2, cntr;
	int ii, jj, i, j;

// Do the VERTICAL transform. DCTImage to tmpbuf and copy back...
	LOG_NEW; float* tmpbuf = new float[(inw > inh ? inw : inh)];
	for(jj = 0; jj < inw; jj++) {
		for(ii = 0; ii < inh; ii += blocksize) {
			ndx = ii*inw + jj;
			for(i = 0; i < blocksize; i++) {
				tmpbuf[ii+i] = 0.0;
				for(cntr = 0; cntr < blocksize; cntr++) {
					tmpbuf[ii+i] += invD(i, cntr) *
							indata[ndx + cntr*inw];
		}	}	}

// Copy data back to main buffer;
		for(i = 0; i < inh; i++) {
			indata[i*inw + jj] = tmpbuf[i];
		}
	} // end for(jj)

// Do HORIZONTAL xform. From indata to tmpbuf to outdata...
	for(ii = 0; ii < outh; ii++) {
		for(jj = 0; jj < inw; jj += blocksize) {
			ndx = ii*inw + jj;
			for(j = 0; j < blocksize; j++) {
				tmpbuf[jj+j] = 0.0;
				for(cntr = 0; cntr < blocksize; cntr++) {
					tmpbuf[jj+j] += invD(j, cntr) * indata[ndx+cntr];
		}	}	}

// Copy to "outdata".
		ndx2 = ii*outw;
		for(j = 0; j < outw; j++) {
			if (tmpbuf[j] < 0.5) outdata[ndx2+j] = (unsigned char) 0;
			else if (tmpbuf[j] > 254.5) outdata[ndx2+j] =
					(unsigned char) 255;
			else outdata[ndx2+j] = (unsigned char) (tmpbuf[j] + 0.5);
		}
	} // end for(ii)

	LOG_DEL; delete [] tmpbuf;
		}
	} // end doInvDCT()

	method {
		name { doFirst }
		type { void }
		arglist { "(DCTImage* di)" }
		access { protected }
		code {
			blocksize = di->retBS();
			LOG_NEW; cosData = new float[blocksize*blocksize];
			cosSet();
		}
	} // end doFirst()

	start { firstTime = 1; }

	wrapup { LOG_DEL; delete [] cosData; }

	go {
// Read input image.
		Envelope inEnvp;
		(input%0).getMessage(inEnvp);
		TYPE_CHECK(inEnvp, "DCTImage");

// Need to call "writableCopy()" rather than "myData()" because the
// doInvDCT function modifies "dctimage"!!
		DCTImage* dctimage = (DCTImage*) inEnvp.writableCopy();
		if (dctimage->fragmented() || dctimage->processed()) {
			LOG_DEL; delete dctimage;
			Error::abortRun(*this,
					"Can't decode a fragmented or processed image.");
			return;
		}

		if (firstTime) { doFirst(dctimage); firstTime = 0; }

		LOG_NEW;
		GrayImage* grayout = new GrayImage((BaseImage&) *dctimage);
		doInvDCT(grayout->retData(), dctimage->retData(),
				grayout->retWidth(), grayout->retHeight(),
				dctimage->fullWidth(), dctimage->fullHeight());

// Since we used writableCopy(), we own "dctimage".
		LOG_DEL; delete dctimage;
		Envelope temp(*grayout);
		output%0 << temp;
	}
} // end defstar{ DctInv }

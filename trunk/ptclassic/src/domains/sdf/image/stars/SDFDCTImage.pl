defstar {
	name		{ Dct }
	domain		{ SDF }
	version		{ $Id$ }
	author		{ Sun-Inn Shih, Paul Haskell }
	copyright	{ 1991 The Regents of the University of California }
	location	{ SDF image library }
	desc {
This star takes GrayImage input and does discrete
cosine transform (DCT) and outputs a DCTImage.
	}
	explanation {
.Id "discrete cosine transform"
.Id "image, discrete cosine transform"
}

	input { name { input } type { message } }
	output { name { output } type { message } }

	defstate {
		name	{ BlockSize }
		type	{ int }
		default { 8 }
		desc	{ Block size of the DCT transform. }
	}

// CODE
	hinclude { <math.h>, "GrayImage.h" ,"DCTImage.h", "Error.h" }

	protected {
		float* cosData;
		int blocksize;
	}

	method {
		name { cosSet } // Fill DCT BASIS matrix
		type { void }
		access { protected }
		code {
			int i, j;
			double lambda = M_PI / (2.0 * blocksize);
			double t1 = 1.0 / sqrt(double(blocksize));
			double t2 = sqrt(2.0 / double(blocksize));
			for(j = 0; j < blocksize; j++) {
				cosData[j] = t1;
				for(i = 1; i < blocksize; i++) {
					cosData[j+i*blocksize] = float(t2 *
							cos(double((2*j+1)*i*lambda)) );
				}
			}
		}
	} // end cosSet()

	inline method {
		name { fwdD }
		type { float }
		arglist { "(int a, int b)" }
		access { protected }
		code { return (cosData[a*blocksize+b]); }
	} // end fwdD()

	method {
		name { doDCT }
		type { void }
		arglist { "(float* out, unsigned const char* in, int outw,
				int outh, int inw, int inh)"
		}
		code {
	register int ndx, ndx2, cntr;
	int ii, jj, i, j;

// Do the HORIZONTAL transform. From input to out...
	for(ii = 0; ii < inh; ii++) {
		for(jj = 0; jj < outw-blocksize; jj += blocksize) {
			ndx = ii*outw + jj;
			ndx2 = ii*inw + jj;
			for(j = 0; j < blocksize; j++) {
				out[ndx+j] = 0.0;
				for(cntr = 0; cntr < blocksize; cntr++) {
					out[ndx+j] += fwdD(j, cntr) * in[ndx2+cntr];
		}	}	}

// Handle last (maybe partial) block at end of each row (i.e. zero-pad).
		ndx = ii*outw + jj;
		ndx2 = ii*inw + jj;
		for(j = 0; j < blocksize; j++) {
			out[ndx+j] = 0.0;
			for(cntr = 0; cntr < 1 + ((inw-1)%blocksize); cntr++) {
				out[ndx+j] += fwdD(j, cntr) * in[ndx2+cntr];
	}	}	}

// Handle rows at the bottom of the array.
	for(ii = inh; ii < outh; ii++) {
		ndx = ii*outw;
		for(j = 0; j < outw; j++) {
			out[ndx+j] = 0.0;
		}
	} // end for(ii)


// Do the VERTICAL transform. From out to tmpbuf and copy back...
	LOG_NEW; float* tmpbuf = new float[outh];
	for(jj = 0; jj < outw; jj++) {
		for(ii = 0; ii < outh; ii+= blocksize) {
			ndx = ii*outw + jj;
			for(i = 0; i < blocksize; i++) {
				tmpbuf[ii+i] = 0.0;
				for(cntr = 0; cntr < blocksize; cntr++) {
					tmpbuf[ii+i] += fwdD(i, cntr) *
							out[ndx + cntr*outw];
		}	}	}

// Copy data back to main buffer;
		for(i = 0; i < outh; i++) {
			out[i*outw+jj] = tmpbuf[i];
		}
	} // end for(jj)

	LOG_DEL; delete [] tmpbuf;
		}
	} // end doDCT()

	start {
		blocksize = int(BlockSize);
		LOG_NEW; cosData = new float[blocksize*blocksize];
		cosSet();
	}

	wrapup { LOG_DEL; delete [] cosData; }

	go {
// Read input image.
		Envelope inEnvp;
		(input%0).getMessage(inEnvp);
		TYPE_CHECK(inEnvp,"GrayImage");
		const GrayImage* image = (const GrayImage*) inEnvp.myData();
		if (image->fragmented() || image->processed()) {
			Error::abortRun(*this,
					"Can't DCT fragmented or processed image.");
			return;
		}

// Do transform.
		LOG_NEW; DCTImage* DCT = new DCTImage(*image, blocksize);
		doDCT(DCT->retData(), image->constData(), DCT->fullWidth(),
				DCT->fullHeight(), image->retWidth(),
				image->retHeight());

// Send output.
		Envelope outEnvp(*DCT);
		output%0 << outEnvp;
	}
} // end defstar{ Dct }

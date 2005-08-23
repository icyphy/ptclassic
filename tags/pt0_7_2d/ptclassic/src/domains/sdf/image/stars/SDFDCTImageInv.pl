defstar {
	name		{ DCTImageInv }
	domain		{ SDF }
	version		{ @(#)SDFDCTImageInv.pl	1.24 01 Oct 1996 }
	author		{ Sun-Inn Shih, Paul Haskell }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
This star takes a float matrix input and does inverse
discrete cosine transform (DCT) coding and outputs a float matrix.
	}
	htmldoc {
<a name="DCT inverse"></a>
<a name="discrete cosine transform inverse"></a>
<a name="image, discrete cosine transform inverse"></a>
<a name="inverse discrete cosine transform"></a>
<a name="inverse DCT"></a>
}

	input { name { input } type { FLOAT_MATRIX_ENV } }
	input { 
	  name { originalW } 
	  type { int }
	  desc { The width of the image before DCT. }
	}
	input { 
	  name { originalH } 
	  type { int }
	  desc { The height of the image before DCT. }
	}
	output { name { output } type { FLOAT_MATRIX_ENV } }

	defstate {
		name	{ BlockSize }
		type	{ int }
		default { 8 }
		desc	{ Block size of the inverse DCT transform. }
	}

	hinclude { <math.h>, "Matrix.h", "Error.h" }

	protected {
		float* cosData;
		int blocksize;
	}

	constructor { cosData = 0; }

	setup { 
		blocksize = int(BlockSize);
		delete [] cosData;
		LOG_NEW; cosData = new float[blocksize*blocksize];
		cosSet();
	}

	destructor { LOG_DEL; delete [] cosData; }

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
		arglist { "(FloatMatrix& out, FloatMatrix& in)" }
		code {
	register int ndx, ndx2, cntr;
	int ii, jj, i, j;

	int outw = out.numCols();
	int outh = out.numRows();
	int inw = in.numCols();
	int inh = in.numRows();

	// Do the VERTICAL transform. image to tmpbuf and copy back...
	LOG_NEW; float* tmpbuf = new float[(inw>inh ? inw : inh) + blocksize];
	for(jj = 0; jj < inw; jj++) {
		for(ii = 0; ii < inh; ii += blocksize) {
			ndx = ii*inw + jj;
			for(i = 0; i < blocksize; i++) {
				double sum = 0.0;
				for(cntr = 0; cntr < blocksize; cntr++) {
					sum += invD(i, cntr) *
					       in.entry(ndx + cntr*inw);
				}
				tmpbuf[ii+i] = sum;
			}
		}

		// Copy data back to main buffer;
		for(i = 0; i < inh; i++) {
			in.entry(i*inw + jj) = tmpbuf[i];
		}
	} // end for(jj)

	// Do HORIZONTAL xform. From indata to tmpbuf to outdata...
	for(ii = 0; ii < outh; ii++) {
		for(jj = 0; jj < inw; jj += blocksize) {
			ndx = ii*inw + jj;
			for(j = 0; j < blocksize; j++) {
				double sum = 0.0;
				for(cntr = 0; cntr < blocksize; cntr++) {
				  sum += invD(j, cntr) * in.entry(ndx+cntr);
				}
				tmpbuf[jj+j] = sum;
			}
		}

		// Copy to "outdata".
		ndx2 = ii*outw;
		for(j = 0; j < outw; j++) {
		  if (tmpbuf[j] < 0.5) {
		    out.entry(ndx2+j) = (unsigned char) 0;
		  }
		  else if (tmpbuf[j] > 254.5) {
		    out.entry(ndx2+j) = (unsigned char) 255;
		  }
		  else {
		    out.entry(ndx2+j) = (unsigned char) (tmpbuf[j] + 0.5);
		  }
		}
	} // end for(ii)

	LOG_DEL; delete [] tmpbuf;
	}
	} // end doInvDCT()

	go {
// Read input image.
                Envelope pkt;
                (input%0).getMessage(pkt);
                const FloatMatrix& inImage = *(const FloatMatrix*)pkt.myData();

		if (pkt.empty()) {
		  Error::abortRun(*this, "Input is a dummyMessage.");
		  return;
		}

		int outw = int(originalW%0);
		int outh = int(originalH%0);

		//Allocate output image.
		LOG_NEW;
		FloatMatrix& outImage = *(new FloatMatrix(outh,outw));

		// Need to allocate a new matrix for inImage
		// because the doInvDCT function modifies the matrix data!!
		LOG_NEW;
		FloatMatrix& tmpImage = *(new FloatMatrix(inImage));

		// Do inverse transform.
		doInvDCT(outImage, tmpImage);

		LOG_DEL; delete &tmpImage;

// Since we used writableCopy(), we own "dctimage".
		output%0 << outImage;
	}
} // end defstar{ DctInv }



defstar {
	name		{ DCTImage }
	domain		{ SDF }
	version		{ @(#)SDFDCTImage.pl	1.23 01 Oct 1996 }
	author		{ Sun-Inn Shih, Paul Haskell }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location	{ SDF image library }
	desc {
This star takes a float matrix input particle,
computes the discrete cosine transform (DCT),
and outputs a float matrix.
	}
	htmldoc {
<a name="discrete cosine transform"></a>
<a name="image, discrete cosine transform"></a>
}

	input { name { input } type { FLOAT_MATRIX_ENV } }
	output { name { output } type { FLOAT_MATRIX_ENV } }
	output { 
	  name { originalW } 
	  type { int }
	  desc { The width of the original (input) image. }
	}
	output { 
	  name { originalH } 
	  type { int }
	  desc { The height of the original (input) image. }
	}


	defstate {
		name	{ BlockSize }
		type	{ int }
		default { 8 }
		desc	{ Block size of the DCT transform. }
	}

	hinclude { <math.h>, "Matrix.h" , "Error.h" }

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
	arglist {	// can't break string across lines in cfront
"(FloatMatrix& outImage, const FloatMatrix& inImage)"
	}
	code {
	register int ndx, ndx2, cntr;
	int ii, jj, i, j;

	int outw = outImage.numCols();
	int outh = outImage.numRows();
	int inw = inImage.numCols();
	int inh = inImage.numRows();

	// Do the HORIZONTAL transform. From input to out...
	for(ii = 0; ii < inh; ii++) {
	  for(jj = 0; jj < outw-blocksize; jj += blocksize) {
	    ndx = ii*outw + jj;
	    ndx2 = ii*inw + jj;
	    for(j = 0; j < blocksize; j++) {
	      double sum = 0.0;
	      for(cntr = 0; cntr < blocksize; cntr++) {
		sum += fwdD(j, cntr) * inImage.entry(ndx2+cntr);
	      }
	      outImage.entry(ndx+j) = sum;
            }
	  }

	// Handle last (maybe partial) block at end of each row (i.e. zero-pad).
	  ndx = ii*outw + jj;
	  ndx2 = ii*inw + jj;
	  for(j = 0; j < blocksize; j++) {
	    double sum = 0.0;
	    for(cntr = 0; cntr < 1 + ((inw-1)%blocksize); cntr++) {
	      sum += fwdD(j, cntr) * inImage.entry(ndx2+cntr);
	    }
	    outImage.entry(ndx+j) = sum;
	  }
	}

// Handle rows at the bottom of the array.
	for(ii = inh; ii < outh; ii++) {
	  ndx = ii*outw;
	  for(j = 0; j < outw; j++) {
	    outImage.entry(ndx+j) = 0.0;
	  }
	} // end for(ii)


// Do the VERTICAL transform. From out to tmpbuf and copy back...
	LOG_NEW; float* tmpbuf = new float[outh + blocksize];
	for(jj = 0; jj < outw; jj++) {
	  for(ii = 0; ii < outh; ii += blocksize) {
	    ndx = ii*outw + jj;
	    for(i = 0; i < blocksize; i++) {
	      double sum = 0.0;
	      for(cntr = 0; cntr < blocksize; cntr++) {
		sum += fwdD(i, cntr) * outImage.entry(ndx + cntr*outw);
	      }
	      tmpbuf[ii+i] = sum;
	    }
	  }

// Copy data back to main buffer;
	  for(i = 0; i < outh; i++) {
	    outImage.entry(i*outw+jj) = tmpbuf[i];
	  }
	} // end for(jj)

	LOG_DEL; delete [] tmpbuf;
	}

	} // end doDCT()

	go {
		// Read input image.
                Envelope pkt;
                (input%0).getMessage(pkt);
                const FloatMatrix& inImage = *(const FloatMatrix*)pkt.myData();

		if (pkt.empty()) {
		  Error::abortRun(*this, "Input is a dummyMessage.");
		  return;
		}

		int inw = inImage.numCols();
		int inh = inImage.numRows();

		//Round output width and height to be multiples of "blocksize"
		int outw;
		int outh;
		if ((inw%blocksize) != 0)
		  outw = blocksize * (inw/blocksize +1);
		else 
		  outw = inw;
		if ((inh%blocksize) != 0)
		  outh = blocksize * (inh/blocksize +1);
		else 
		  outh = inh;

		//Allocate output image.
		LOG_NEW;
		FloatMatrix& outImage = *(new FloatMatrix(outh,outw));

		// Do transform.
		doDCT(outImage, inImage);

		// Send output.
		output%0 << outImage;
		originalW%0 << inw;
		originalH%0 << inh;
	}
} // end defstar{ Dct }

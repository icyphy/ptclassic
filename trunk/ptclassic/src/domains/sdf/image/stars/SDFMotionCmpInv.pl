defstar {
  name { MotionCmpInv }
  domain { SDF }
  version { @(#)SDFMotionCmpInv.pl	1.23 01 Oct 1996 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
For NULL inputs (zero size matrices) on 'mvHorzIn' and/or 'mvVertIn' 
(motion-vector-in), copy the 'diffIn' input unchanged to the output 
and discard the 'pastIn' input. (A NULL input usually indicates the 
first frame of a sequence.)

For non-NULL 'mvHorzIn' and 'mvVertIn' inputs, perform inverse motion
compensation and write the result to 'output'.
  }
	htmldoc {
<a name="motion compensation, inverse"></a>
<a name="inverse motion compensation"></a>
  }

  //////// I/O AND STATES.
  input { name { diffIn } type { FLOAT_MATRIX_ENV } }

  input { name { mvHorzIn } type { FLOAT_MATRIX_ENV } }
  input { name { mvVertIn } type { FLOAT_MATRIX_ENV } }

  input { name { pastIn } type { FLOAT_MATRIX_ENV } }

  output { name { output } type { FLOAT_MATRIX_ENV } }

  defstate {
    name { BlockSize }
    type { int }
    default { 8 }
    desc { Size of blocks on which to do inverse motion comp. }
  }

  ////// CODE.
  hinclude { "Matrix.h", "Error.h" }

  method {
    name { doInvMC }
    type { "void" }
    access { private }
    arglist { "(FloatMatrix& out, const FloatMatrix& prev, const FloatMatrix& diff, const FloatMatrix& horzImg, const FloatMatrix& vertImg, const int width, const int height, const int blocksize)" }
    code {
      int ii, jj;
      int count = 0;
      for(ii = 0; ii < height; ii += blocksize) {
	for(jj = 0; jj < width; jj += blocksize) {
	  char horz = (char)horzImg.entry(count);
	  char vert = (char)vertImg.entry(count);
	  if(BlockIsLost(diff, ii, jj, blocksize, width)) {
	    DoLostBlock(out, prev, horz, vert, 
			ii, jj, blocksize, width);
	  } else {
	    DoOneBlock(out, prev, diff, horz, vert, 
		       ii, jj, blocksize, width);
	  }
	  count++;
	}
      }
    }
  } // end doInvMC()

  virtual method {
    name { BlockIsLost }
    access { protected }
    arglist{ "(const FloatMatrix& ptr, const int ii, const int jj, const int blocksize, const int width)" }
    type { "int" }
    code {
      int i, j, indx;
      for(i = 0; i < blocksize; i++) {
	indx = jj + (i+ii)*width;
	for(j = 0; j < blocksize; j++) {
	  if (ptr.entry(indx + j) != 0) {
	    return 0;
	  }
	}
      }
      return 1;
    }
  }

  virtual method {
    name { DoOneBlock }
    type { "void" }
    access { protected }
    arglist { "(FloatMatrix& out, const FloatMatrix& prev, const FloatMatrix& diff, const char horz, const char vert, const int ii, const int jj, const int blocksize, const int width)" }
    code {
      int i, j, index, mcindex;
      for(i = 0; i < blocksize; i++) {
	index = jj + (i+ii)*width;
	// NOTE THE MINUS '-' SIGNS BELOW. These are because the
	// motion vector points FROM the old block TO the current.
	mcindex = index - int(horz) - (width * int(vert));
	for(j = 0; j < blocksize; j++) {
	  out.entry(index+j) = 
	        quant(int(diff.entry(index+j)), int(prev.entry(mcindex+j)));
	}
      }
    }
  } // end DoOneBlock{}

  virtual method {
    name { DoLostBlock }
    type { "void" }
    access { protected }
    arglist { "(FloatMatrix& out, const FloatMatrix& prev, const char horz, const char vert, const int ii, const int jj, const int blocksize, const int width)" }
    code {
      int i, j, index, mcindex;
      for(i = 0; i < blocksize; i++) {
	index = jj + (i+ii)*width;
	// NOTE THE MINUS '-' SIGNS BELOW. These are because the
	// motion vector points FROM the old block TO the current.
	mcindex = index - int(horz) - (width * int(vert));
	for(j = 0; j < blocksize; j++) {
	  out.entry(index+j) = prev.entry(mcindex+j);
	}
      }
    }
  } // end DoLostBlock{}

// See SDFMotionCmp::quant()
// That function divides the true difference by 2 so it will fit into
// 8 bits.  It also adds 128 so negative numbers don't wrap around.
	inline virtual method {
		name { quant }
		type { "unsigned char" }
		access { protected }
		arglist { "(const int i1, const int i2)" }
		code {
			int f = 2 * (i1 - 128) + i2;
			if (f < 0) { return ((unsigned char) 0); }
			else if (f > 254) { return ((unsigned char) 255); }
			else { return((unsigned char) f); }
	}	}

  method {
    name { sizesMatch }
    type { "int" }
    access { private }
    arglist { "(const FloatMatrix& im1, const FloatMatrix& im2, const FloatMatrix& mvH, const FloatMatrix& mvV)" }
    code {
      int w = im1.numCols();
      int h = im1.numRows();
      int retval = (w == im2.numCols());
      retval &= (h == im2.numRows());

      int bs = int(BlockSize);
      retval &= (w == bs*mvH.numCols());
      retval &= (h == bs*mvH.numRows());
      retval &= (w == bs*mvV.numCols());
      retval &= (h == bs*mvV.numRows());

      return(retval);
    }
  }

  go {
  // Read data from inputs and initialize
    Envelope difEnvp, pastEnvp;
    (diffIn%0).getMessage(difEnvp);
    (pastIn%0).getMessage(pastEnvp);
    const FloatMatrix& diffImg = *(const FloatMatrix *)difEnvp.myData();
    const FloatMatrix& pastImg = *(const FloatMatrix *)pastEnvp.myData();

    Envelope mvHorzEnvp,mvVertEnvp;
    (mvHorzIn%0).getMessage(mvHorzEnvp);
    (mvVertIn%0).getMessage(mvVertEnvp);
    const FloatMatrix& mvHorzImg = *(const FloatMatrix *)mvHorzEnvp.myData();
    const FloatMatrix& mvVertImg = *(const FloatMatrix *)mvVertEnvp.myData();
     
    // Handle the first input image.
    if ( pastEnvp.empty()  ||
	 mvHorzImg.numRows()*mvHorzImg.numCols() == 0  ||
	 mvVertImg.numRows()*mvVertImg.numCols() == 0  ) {
      output%0 << difEnvp;
      return;
    }

    if(!sizesMatch(diffImg, pastImg, mvHorzImg, mvVertImg)) {
      Error::abortRun(*this, "Size mismatch");
      return;
    }

    // If we reached here, we have a motion-compensated image.
    // Create output image. 
    int height = diffImg.numRows();
    int width  = diffImg.numCols();
    LOG_NEW;
    FloatMatrix& outImage = *(new FloatMatrix(height,width));

    // Do the inverse motion compensation and send output.
    doInvMC(outImage, pastImg, diffImg, mvHorzImg,
	    mvVertImg, width, height, int(BlockSize));

    output%0 << outImage;
  } // end go{}
} // end defstar { MotionCmpInv }

defstar {
  name { DCTImgCdeInv }
  domain { SDF }
  version { $Id$ }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
This star reads two coded DCT images (one high priority and one low-priority),
which are represented by two input FloatMatrix particle. 
inverts the run-length encoding, and outputs the resulting image.
Protection is built in to avoid crashing even if some of the coded
input data is affected by loss.

  }
  seealso { DCTImageCode }
  
  input	{ name	{ hiport }	type { FLOAT_MATRIX_ENV } }
  input	{ name	{ loport }	type { FLOAT_MATRIX_ENV } }

  input { 
    name { originalW } 
    type { int }
    desc { The width of the original image before DCTImageCode. }
  }
  input { 
    name { originalH } 
    type { int }
    desc { The height of the original image before DCTImageCode. }
  }

  output { name	{ output }	type { FLOAT_MATRIX_ENV } }

  defstate {
    name { BlockSize }
    type { int }
    default { 8 }
    desc { Block size of the DCT transformed image. }
  }

  defstate {
    name	{ HiPri }
    type	{ int }
    default	{ 1 }
    desc	{ Number of DCT coeffs per block to 'highport' input. }
  }

  //// CODE
  hinclude { "Matrix.h", "Error.h", "ptdspRunLength.h" }

  method { // Do the run-length decoding.
    name { invRunLen }
    type { "FloatMatrix&" }
    access { protected }
    arglist { "(const FloatMatrix& hiImage, const FloatMatrix& loImage, int width, int height)" }
    code {
      // Initialize.
      const int fullFrame = width * height;
      const double* hiImagePtr = hiImage[0];
      const double* loImagePtr = loImage[0];

      double* outPtr = new double[fullFrame];
      for(int k = 0; k < fullFrame; k++) { outPtr[k] = 0.0; }
      
      Ptdsp_RunLengthInverse (hiImagePtr, loImagePtr, outPtr, fullFrame, 
			      int(BlockSize), loImage.numRows() * loImage.numCols(),
			      HiPri);

      // Copy the data to return.
      FloatMatrix& outImage = *(new FloatMatrix(height,width));
      for(int i = 0; i < fullFrame; i++) {
	outImage.entry(i) = outPtr[i];
      }
      delete [] outPtr;

      return outImage;
    }
  } // end { invRunLen }
  
  go {
    // Read input images.
    Envelope hiEnvp;
    (hiport%0).getMessage(hiEnvp);
    const FloatMatrix& hiImage = *(const FloatMatrix *) hiEnvp.myData();

    Envelope loEnvp;
    (loport%0).getMessage(loEnvp);
    const FloatMatrix& loImage = *(const FloatMatrix *) loEnvp.myData();

    if ( hiEnvp.empty() || loEnvp.empty() ) {
      Error::abortRun(*this, "Input is a dummyMessage.");
      return;
    }

    // Do the conversion.
    FloatMatrix& outImage = invRunLen(hiImage, loImage,
                                      int(originalW%0),int(originalH%0));

    // Send output.
    output%0 << outImage;
  }
} // end defstar{ DctImgCdeInv }

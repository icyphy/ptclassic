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
  hinclude { "Matrix.h", "Error.h" }

  code {
// Numbers unlikely to come from a DCT...
	  const float StartOfBlock = 524288.0;
	  const float StartOfRun = 1048576.0;
  }


  method { // Do the run-length decoding.
    name { invRunLen }
    type { "FloatMatrix&" }
    access { protected }
    arglist { "(const FloatMatrix& hiImage, const FloatMatrix& loImage, int width, int height)" }
    code {
      // Initialize.
      const int bSize = int(BlockSize);
      const int fullFrame = width * height;

      // Do DC image first.
      int i, j, k, blk;

      float* outPtr = new float[fullFrame];
      for(k = 0; k < fullFrame; k++) { outPtr[k] = 0.0; }
      
      i = 0;
      for(j = 0; j < fullFrame; j += bSize * bSize) {
	for(k = 0; k < HiPri; k++) {
	  outPtr[j + k] = hiImage.entry(i++);
	}
      }
      
      // While still low priority input data left...
      const int size = loImage.numRows() * loImage.numCols();
      
      i = 0;
      while (i < size) {

	// Process each block, which starts with "StartOfBlock".
	while ((i < size) && (loImage.entry(i) != StartOfBlock)) {
	  i++;
	}
	if (i < size-2) {
	  i++;
	  blk = int(loImage.entry(i++));
	  blk *= bSize*bSize;
	  if ((blk >= 0) && (blk < fullFrame)) {
	    blk += HiPri;
	    k = 0;
	    while ((i < size) && (k < bSize*bSize - HiPri)
		   && (loImage.entry(i) != StartOfBlock)) {
	      if (loImage.entry(i) == StartOfRun) {
		i++;
		if (i < size) {
		  int runLen = int(loImage.entry(i++));
		  for(int l = 0; l < runLen; l++ ) {
		    outPtr[blk+k] = 0.0;
		    k++;
		  }
		}
	      } else {
		outPtr[blk+k] = loImage.entry(i++);
		k++;
	      }
	    }
	  } // if (blk OK)
	}
      } // end while (indx < size)
      
      // Copy the data to return.
      FloatMatrix& outImage = *(new FloatMatrix(height,width));
      for(i = 0; i < fullFrame; i++) {
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

defstar {
  name { DCTImgCde }
  domain { SDF }
  version { $Id$ }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
This star takes a Float Matrix which represents a DCT image, 
inserts "start of block" markers, run-length encodes it, 
and outputs the modified image. 
For the run-length encoding, all values with absolute value less than "Thresh"
are set to 0.0, to help improve compression.

Runlengths are coded with a "start of run" symbol and then an (integer) 
run-length.
"HiPri" DCT coefficients per block are sent to "hiport", the high-priority 
output. 
The remainder of the coefficients are sent to "loport", the low-priority 
output.
  }

  input	{ name { inport }	type { FLOAT_MATRIX_ENV } }

  output { name { hiport }	type { FLOAT_MATRIX_ENV } }
  output { name { loport }	type { FLOAT_MATRIX_ENV } }

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

  output { name { compr }	type { float } }

  defstate {
    name { BlockSize }
    type { int }
    default { 8 }
    desc { Block size of the input DCT transformed image. }
  }

  defstate {
    name	{ Thresh }
    type	{ float }
    default { 12.0 }
    desc	{ Threshold for run-length coding. }
  }

  defstate {
    name	{ HiPri }
    type	{ int }
    default { 3 }
    desc { Number of DCT coefficients per block sent to 'hiport'. }
  }

  // CODE
  hinclude { "Matrix.h", "Error.h" }

  code {
    const float StartOfBlock = 524288.0;
    const float StartOfRun = 1048576.0;
  }
  protected {
    float thresh;    
    float *outDc, *outAc;
    int indxDc, indxAc;
  }

  constructor {
    outDc = 0;
    outAc = 0;
  }

  setup {
    thresh = float(fabs(double(Thresh)));
  }

  method { // Do the run-length coding.
    name { doRunLen }
    type { "void" }
    access { protected }
    arglist { "(const FloatMatrix& inImage)" }
    code {
      // Initialize.
      const int bSize = int(BlockSize);
      const int size = inImage.numCols() * inImage.numRows();
      const int blocks = size / (bSize*bSize);

      // Temporary storage for one block.
      float* tmpFloatPtr = new float[size];
      float* tmpPtr = tmpFloatPtr;
      for (int i = 0; i < size; i++) {
	tmpPtr[i] = float(inImage.entry(i));
      }

      // The biggest runlen blowup we can have is the string "01010101...".
      // This gives a blowup of 50%, so with StartOfBlock and StartOfRun
      // markers, 1.70 should be ok.
      delete [] outDc;
      delete [] outAc;
      outDc = new float[int(1.70*size + 1)];
      outAc = new float[int(1.70*size + 1)];
      indxDc = 0; indxAc = 0;
      for (int blk = 0; blk < blocks; blk++) {
	// High priority coefficients.
	for(i = 0; i < HiPri; i++) {
	  outDc[indxDc++] = *tmpPtr++;
	}
	
	// Low priority coefficients--start with block header.
	outAc[indxAc++] = StartOfBlock;
	outAc[indxAc++] = float(blk);
	
	int zeroRunLen = 0;
	for(; i < bSize*bSize; i++) {
	  if(zeroRunLen) {
	    if (larger(*tmpPtr, thresh)) {
	      outAc[indxAc++] = float(zeroRunLen);
	      zeroRunLen = 0;
	      outAc[indxAc++] = *tmpPtr;
	    } else {
	      zeroRunLen++;
	    }
	  } else {
	    if (larger(*tmpPtr, thresh)) {
	      outAc[indxAc++] = *tmpPtr;
	    } else {
	      outAc[indxAc++] = StartOfRun;
	      zeroRunLen++;
	    }
	  }
	  tmpPtr++;
				}
	// Handle zero-runs that last till end of the block.
	if(zeroRunLen) {
	  outAc[indxAc++] = float(zeroRunLen);
	}
      }

      // Delete tmpFloatPtr and not tmpPtr since tmpPtr has been incremented
      delete [] tmpFloatPtr;
    }
  }


  inline method {
    name { larger }
    type { "int" }
    access { protected }
    arglist { "(const float fl, const float in)" }
    code { return (fabs(double(fl)) >= double(in)); }
  }


  go {
    // Read input.
    Envelope inEnvp;
    (inport%0).getMessage(inEnvp);
    const FloatMatrix& inImage = *(const FloatMatrix *) inEnvp.myData();

    if (inEnvp.empty()) {
      Error::abortRun(*this, "Input is a dummyMessage.");
      return;
    }

    // Do processing and send out.
    doRunLen(inImage);

    // Copy the data to the output images.
    FloatMatrix& dcImage = *(new FloatMatrix(1,indxDc));
    for(int i = 0; i < indxDc; i++) {
      dcImage.entry(i) = outDc[i];
    }
    delete [] outDc;
    outDc = 0;

    FloatMatrix& acImage = *(new FloatMatrix(1,indxAc));
    for(int j = 0; j < indxAc; j++) {
      acImage.entry(j) = outAc[j];
    }
    delete [] outAc;
    outAc = 0;

    float comprRatio = dcImage.numRows() * dcImage.numCols() +
                       acImage.numRows() * acImage.numCols();
    comprRatio /= inImage.numRows()*inImage.numCols();
    compr%0 << comprRatio;

    hiport%0 << dcImage;
    loport%0 << acImage;
    originalW%0 << inImage.numCols();
    originalH%0 << inImage.numRows();
  }

  destructor {
    delete [] outDc;
    delete [] outAc;
  }
} // end defstar { DctImgCde }

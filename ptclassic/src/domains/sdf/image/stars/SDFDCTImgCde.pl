defstar {
  name { DCTImgCde }
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
  hinclude { "Matrix.h", "Error.h", "ptdspRunLength.h" }

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

  go {
    // Read input.
    Envelope inEnvp;
    (inport%0).getMessage(inEnvp);
    FloatMatrix& inImage = *(FloatMatrix *) inEnvp.myData();

    if (inEnvp.empty()) {
      Error::abortRun(*this, "Input is a dummyMessage.");
      return;
    }

    // transfer matrix information from FloatMatrix to Ptdsp_FloatMatrix_t,
    // before passing to Ptdsp_RunLengthEncode
    Ptdsp_FloatMatrix_t inImageMatrix;

    inImageMatrix = Ptdsp_FloatMatrixAllocRows (inImage.numRows(), inImage.numCols());
    // copy each row pointer over, not entire contents of the matrix
    for (int i = 0; i < inImage.numRows(); i++) {
      inImageMatrix.matrix[i] = inImage[i];
    }

    // Do processing and send out.
    Ptdsp_RunLengthEncode (inImageMatrix, BlockSize, HiPri, thresh, &outDc, &outAc, 
			   &indxDc, &indxAc);

    // Copy the data to the output images.
    FloatMatrix& dcImage = *(new FloatMatrix(1,indxDc));
    for ( i = 0; i < indxDc; i++) {
      dcImage.entry(i) = outDc[i];
    }

    FloatMatrix& acImage = *(new FloatMatrix(1,indxAc));
    for(int j = 0; j < indxAc; j++) {
      acImage.entry(j) = outAc[j];
    }

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

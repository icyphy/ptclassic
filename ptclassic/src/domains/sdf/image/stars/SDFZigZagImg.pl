defstar {
  name { ZigZagImg }
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
This star zig-zag scans a FloatMatrix and outputs the result.
This is useful before quantizing a DCT transformed image.
  }

  input	{ name { inport } type { FLOAT_MATRIX_ENV } }
  output { name { outport } type { FLOAT_MATRIX_ENV } }

  defstate {
    name { BlockSize }
    type { int }
    default { 8 }
    desc { Block size of each zig-zag scan. }
  }

  hinclude { "Matrix.h", "Error.h" }
  ccinclude { "ptdspZigZag.h" }

  method {
    name { doZigZag }
    type { "void" }
    access { protected }
    arglist { "(const FloatMatrix& inImg, FloatMatrix& outImg)" }
    code {
      // initialize
      double * outArr = new double[inImg.numCols() * inImg.numRows()];

      // set outArr to the array representing inImage
      // inImage[0] returns address of 1st row, which is also address of the array
      // since array is contiguously stored in memory
      const double* inImagePtr = inImg[0];

      Ptdsp_ZigZagScan ( inImagePtr, outArr, inImg.numCols(), inImg.numRows(), 
			 int(BlockSize));

      // Copy the data to the outImg.
      for(int i = 0; i < inImg.numCols() * inImg.numRows(); i++) {
	outImg.entry(i) = outArr[i];
      }
      LOG_DEL; delete [] outArr;
    }
  } // end { doZigZag }

  go {
    Envelope inEnvp;
    (inport%0).getMessage(inEnvp);
    const FloatMatrix& inImg = *(const FloatMatrix*)inEnvp.myData();

    if (inEnvp.empty()) {
      Error::abortRun(*this, "Input is a dummyMessage.");
      return;
    }

    // Allocate output image.
    FloatMatrix& outImg = *(new FloatMatrix(inImg));

    // Do processing and send out.
    doZigZag(inImg,outImg);
    outport%0 << outImg;
  }
} // end defstar { ZigZagImg }

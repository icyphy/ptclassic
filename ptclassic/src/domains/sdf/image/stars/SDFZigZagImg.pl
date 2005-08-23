defstar {
  name { ZigZagImg }
  domain { SDF }
  version { @(#)SDFZigZagImg.pl	1.19 7/12/96 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
      int width = inImg.numCols();
      int height = inImg.numRows();
      double * outArr = new double[inImg.numCols() * inImg.numRows()];

      // FIXME
      // Sets inImagePtr to the vector representing the FloatMatrix.
      // This only works because in the underlying implementation of FloatMatrix,
      // inImage[0], which returns the 1st row, also returns the entire vector
      // representing the matrix. 
      // A method should be added to the FloatMatrix class to do this instead
      // of relying on this current operation
      const double* inImagePtr = inImg[0];

      Ptdsp_ZigZagScan ( inImagePtr, outArr, width, height, int(BlockSize));

      // Copy the data to the outImg.
      for(int i = 0; i < width * height; i++) {
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

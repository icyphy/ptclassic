defstar {
  name { ZigZagImgInv }
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
This star inverse zig-zag scans a Float Matrix.
  }
  seealso { ZigZagImg }

  input	{ name	{ inport }	type { FLOAT_MATRIX_ENV } }
  output { name	{ outport }	type { FLOAT_MATRIX_ENV } }

  defstate {
    name { BlockSize }
    type { int }
    default { 8 }
    desc { Block size of each inverse zig-zag scan. }
  }
  hinclude { "Matrix.h", "Error.h" }
  ccinclude { "ptdspZigZag.h" }

  method { 
    name { invZigZag }
    type { "void" }
    access { protected }
    arglist { "(const FloatMatrix& inImg, FloatMatrix& outImg)" }
    code {
      // Initialize.
      int width = inImg.numCols();
      int height = inImg.numRows();

      // Allocate space and go.
      double* outFloatArr = new double[width*height];
      double* outArr = outFloatArr;

      // set tmpFloatPtr to the array representing inImg
      // inImg[0] returns address of 1st row, which is also address of the array
      // since array is contiguously stored in memory
      const double* tmpFloatPtr = inImg[0];

      Ptdsp_ZigZagInverse ( tmpFloatPtr, outArr, width,
			    height, int(BlockSize));

      // Copy the data to the outImg.
      for(int i = 0; i < width * height; i++) {
	outImg.entry(i) = *outArr++;
      }

      // Deallocate memory
      delete [] outFloatArr;
    }
  }

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
    invZigZag(inImg, outImg);
    outport%0 << outImg;
  }
}

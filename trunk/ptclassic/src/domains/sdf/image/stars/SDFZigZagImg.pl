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


  method { // zig-zag scan one block. "fData" holds output.
    name { zigzag }
    type { "void" }
    access { protected }
    arglist { "(float* fData, const FloatMatrix& imData, const int i, const int j, const int width, const int blockSize)" }
    code {
      int k, l, indx;

      // Do zig-zag scan.
      indx = 0;
      // K is length of current (semi)diagonal; L is iteration along diag.
      for(k = 1; k < blockSize; k++) { // Top triangle
	for(l = 0; l < k; l++) { // down
	  fData[indx++] = imData.entry(j + (i+l)*width + (k-l-1));
	}
	k++; // NOTE THIS!
	for(l = 0; l < k; l++) { // back up
	  fData[indx++] = imData.entry(j + (i+k-l-1)*width + l);
	}
      }

      // If blockSize an odd number, start with diagonal, else one down.
      if (blockSize % 2) { k = blockSize; }
      else { k = blockSize-1; }

      for(; k > 1; k--) { // Bottom triangle
	for(l = 0; l < k; l++) { // down
	  fData[indx++] = imData.entry(j + (i+blockSize-k+l)*width +
				 (blockSize-l-1));
	}
	k--; // NOTE THIS!
	for(l = 0; l < k; l++) { // back up
	  fData[indx++] = imData.entry(j + (i+blockSize-l-1)*width +
				 blockSize-k+l);
	}
      }

      // Have to do last element.
      fData[indx] = imData.entry(j + (i + blockSize - 1) * width +
			   blockSize - 1);
    } // end code {}
  } // end zigzag {}


  method {
    name { doZigZag }
    type { "void" }
    access { protected }
    arglist { "(const FloatMatrix& inImg, FloatMatrix& outImg)" }
    code {
      // Initialize.
      const int bSize = int(BlockSize);
      const int width = inImg.numCols();
      const int height = inImg.numRows();

      float* outArr = new float[width*height];

      // For each row and col...
      int row, col;
      float* tmpPtr = outArr;
      for(row = 0; row < height; row += bSize) {
	for(col = 0; col < width; col += bSize) {
	  zigzag(tmpPtr, inImg, row, col, width, bSize);
	  tmpPtr += bSize*bSize;
	}
      } // end for(each row and column)

      // Copy the data to the outImg.
      for(int i = 0; i < width*height; i++) {
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

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

  method {		// invert zig-zag scan. "imData" holds output.
    name { ziginv }
    type { "void" }
    access { protected }
    arglist {	// can't break string across lines in cfront
"(float* imData, const float* fData, const int i, const int j, const int width, const int blockSize)"
    }
    code {
      int k, l, indx;

      // Invert the zigzag.
      indx = 0;
      // K is length of current (semi)diagonal; L is iteration on diag.
      for(k = 1; k < blockSize; k++) { // Top triangle
	for(l = 0; l < k; l++) { // down
	  imData[j + (i+l)*width + (k-l-1)] = fData[indx++];
	}
	k++; // NOTE THIS!
	for(l = 0; l < k; l++) { // back up
	  imData[j + (i+k-l-1)*width + l] = fData[indx++];
	}
      }

      // If blockSize an odd number, start with diagonal, else one down.
      if (blockSize % 2) { k = blockSize; }
      else { k = blockSize-1; }
      
      for(; k > 1; k--) { // Bottom triangle
	for(l = 0; l < k; l++) { // down
	  imData[j + (i+blockSize-k+l)*width +
		 (blockSize-l-1)] = fData[indx++];
	}
	k--; // NOTE THIS!
	for(l = 0; l < k; l++) { // back up
	  imData[j + (i+blockSize-l-1)*width +
		 blockSize-k+l] = fData[indx++];
	}
      }

      // Have to do last element.
      imData[j + (i + blockSize - 1) * width + blockSize - 1] = fData[indx];
    } // end code {}
  } // end method { ziginv }


  method { 
    name { invZigZag }
    type { "void" }
    access { protected }
    arglist { "(const FloatMatrix& inImg, FloatMatrix& outImg)" }
    code {
      // Initialize.
      int bSize = int(BlockSize);
      int width = inImg.numCols();
      int height = inImg.numRows();

      // Allocate space and go.
      float* outArr = new float[width*height];
      float* tmpPtr = new float[width*height];

      // Copy the data from the inImg.
      for(int i = 0; i < width*height; i++) {
	tmpPtr[i] = inImg.entry(i);
      }

      for(int row = 0; row < height; row += bSize) {
	for(int col = 0; col < width; col += bSize) {
	  ziginv(outArr, tmpPtr, row, col, width, bSize);
	  tmpPtr += bSize*bSize;
	}
      }

      // Copy the data to the outImg.
      for(i = 0; i < width*height; i++) {
	outImg.entry(i) = outArr[i];
      }

      // Deallocate memory
      delete [] tmpPtr;
      delete [] outArr;
    }
  } // end { invZigZag }


	method {
		name { copy }
		type { void }
		arglist { "(const int c, float* to, const float* from)" }
		access { protected }
		code {
			for(int i = 0; i < c % 5; i++) { *to++ = *from++; }

			for(i = c % 5; i < c; i += 5) {
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
				*to++ = *from++;
			}
	}	}


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

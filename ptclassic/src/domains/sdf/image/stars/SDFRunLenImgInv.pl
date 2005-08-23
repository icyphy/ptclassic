defstar {
  name { RunLenImgInv }
  domain { SDF }
  version { @(#)SDFRunLenImgInv.pl	1.19 01 Oct 1996 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
Accept a Float Matrix and inverse run-length encode it.
Check to make sure we don't write past unallocated memory.
  }
	htmldoc {
<a name="decompression, run length"></a>
<a name="decoding, run length"></a>
<a name="run length decoding"></a>
<a name="image, run length decoding"></a>
  }
  seealso { RunLenImg }

  hinclude { "Matrix.h", "Error.h" }

  // INPUT AND STATES.
  input { name { input } type { FLOAT_MATRIX_ENV } }
  input { 
    name { originalW } 
    type { int }
    desc { The width of the original image before run-length encoded. }
  }
  input { 
    name { originalH } 
    type { int }
    desc { The height of the original image before run-length encoded. }
  }
  output { name { output } type { FLOAT_MATRIX_ENV } }

  defstate {
    name { meanVal }
    type { int }
    default { 0 }
    desc { Center value for thresholding. }
  }

  // Note: we need to check for indx2 < fullFrame in case of lost data.
  // Otherwise we might crash the program when we write past allocated
  // memory.
  method {
    name { invRunLen }
    type { "FloatMatrix&" }
    access { protected }
    arglist { "(const FloatMatrix& inData,int outW, int outH)" }
    code {
      // Do the run-length coding.
      const int size = inData.numRows() * inData.numCols();
      const int fullFrame = outW * outH;

      LOG_NEW; unsigned char* ptr2 = new unsigned char[fullFrame];

      int indx1 = 0, indx2 = 0, count;
      while ((indx1 < size) && (indx2 < fullFrame)) {
        // Do a zero-run.
	while ( (indx1 < size)  && 
                (inData.entry(indx1) == int(meanVal))  &&
	        (indx2 < fullFrame) ) {
	  indx1++; // Skip to run length.
	  for(count = 0; (count < int(inData.entry(indx1)))  &&
	      (indx2 < fullFrame); count++) {
            ptr2[indx2++] = (unsigned char) int(meanVal);
	  }
	  indx1++; // Skip past run length.
	}
        // Handle a non-zero run.
	while ((indx1 < size) && (inData.entry(indx1) != int(meanVal))
		&& (indx2 < fullFrame)) {
	  ptr2[indx2++] = (unsigned char)(inData.entry(indx1++));
	}
      }

      // Copy the data to return.
      LOG_NEW; FloatMatrix& outImage = *(new FloatMatrix(outH,outW));
      for(indx1 = 0; indx1 < indx2; indx1++) {
	outImage.entry(indx1) = ptr2[indx1];
      }
      for(; indx1 < fullFrame; indx1++) {
	outImage.entry(indx1) = (unsigned char) 0;
      }
      LOG_DEL; delete [] ptr2;

      return outImage;
    } // end { invRunLen }
  }

  go {
    // Read input.
    Envelope inEnvp;
    (input%0).getMessage(inEnvp);
    const FloatMatrix& inData = *(const FloatMatrix *) inEnvp.myData();

    if (inEnvp.empty()) {
      Error::abortRun(*this, "Input is a dummyMessage.");
      return;
    }

    // Do processing and send out.
    FloatMatrix& outImg = invRunLen(inData,int(originalW%0),int(originalH%0));
    output%0 << outImg;
  }
} // end defstar { RunLenImgInv }

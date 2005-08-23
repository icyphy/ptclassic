defstar {
  name { RunLenImg }
  domain { SDF }
  version { @(#)SDFRunLenImg.pl	1.20 01 Oct 1996 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
Accept a Float Matrix and run-length encode it. All values closer than
"Thresh" to "meanVal" are set to "meanVal" to help improve compression.

Run lengths are coded with a start symbol of "meanVal" and then a
run-length between 1 and 255. Runs longer than 255 must be coded in
separate pieces.
  }
	htmldoc {
<a name="compression, run length"></a>
<a name="encoding, run length"></a>
<a name="run length encoding"></a>
<a name="image, run length encoding"></a>
  }

  hinclude { "Matrix.h", "Error.h" }

  // INPUT AND STATES.
  input { name { input } type { FLOAT_MATRIX_ENV } }
  output { name { output } type { FLOAT_MATRIX_ENV } }
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
  output { name { compression } type { float } }

  defstate {
    name	{ Thresh }
    type	{ int }
    default { 0 }
    desc	{ Threshold for run-length coding. }
  }
  defstate {
    name { meanVal }
    type { int }
    default { 0 }
    desc { Center value for thresholding. }
  }

  // CODE.
  protected { int thresh; }
  setup { thresh = int(Thresh); }

  inline method {
    name { gt }
    type { "int" }
    access { protected }
    arglist { "(unsigned char ch, int in)" }
    code {
      return (abs(int(ch) - int(meanVal)) > in);
    }
  }

  method {
    name { doRunLen }
    type { "FloatMatrix&" }
    access { protected }
    arglist { "(const FloatMatrix& inImage)" }
    code {
      // Do the run-length coding.
      const int size = inImage.numRows() * inImage.numCols();

      // The biggest blowup we can have is the string "01010101...".
      // This gives a blowup of 33%, so 1.34 is ok.
      LOG_NEW;
      unsigned char* ptr2 = new unsigned char[int(1.34*size + 1)];
      
      int indx1 = 0, indx2 = 0;
      while (indx1 < size) {
	while ( indx1 < size   && 
	        gt((unsigned char)inImage.entry(indx1),thresh) ) {
	  ptr2[indx2++] = (unsigned char)inImage.entry(indx1++);
	}
	if (indx1 < size) {
	  int thisRun = 1; indx1++;
	  while ( (indx1 < size) && (thisRun < 255) &&
		  (!gt((unsigned char)inImage.entry(indx1),thresh)) ) {
	    indx1++; thisRun++;
	  }
	  ptr2[indx2++] = (unsigned char) int(meanVal);
	  ptr2[indx2++] = (unsigned char) thisRun;
	}
      }

      // Copy the data to return.
      LOG_NEW; FloatMatrix& outData = *(new FloatMatrix(1,indx2));
      for(indx1 = 0; indx1 < indx2; indx1++) {
	outData.entry(indx1) = ptr2[indx1];
      }
      LOG_DEL; delete [] ptr2;

      return outData;
    } // end { doRunLen }
  }

  go {
    // Read input.
    Envelope inEnvp;
    (input%0).getMessage(inEnvp);
    const FloatMatrix& inImage = *(const FloatMatrix *) inEnvp.myData();

    if (inEnvp.empty()) {
      Error::abortRun(*this, "Input is a dummyMessage.");
      return;
    }

    // Do processing and send out.
    FloatMatrix& outData = doRunLen(inImage);
    const float comprRatio = float(outData.numRows() * outData.numCols()) /
      float(inImage.numRows() * inImage.numCols());
    compression%0 << comprRatio;
    
    output%0 << outData;
    originalW%0 << inImage.numCols();
    originalH%0 << inImage.numRows();

  }
} // end defstar { RunLenImg }

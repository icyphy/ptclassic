defstar {
  name { DPCMImage }
  domain { SDF }
  version { @(#)SDFDPCMImage.pl	1.17 01 Oct 1996 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
This star implements differential pulse code modulation of an image.
If the "past" input is not a Float Matrix, pass the "input"
directly to the "output". Otherwise, subtract the "past" from the
"input" (with leakage factor alpha) and send the result to "output".
  }
	htmldoc {
<a name="image DPCM"></a>
<a name="image compression"></a>
<a name="DPCM, image"></a>
<a name="compression, image"></a>
  }

  hinclude { "Matrix.h", "Error.h" }


  //////// I/O AND STATES.
  input { name { input } type { FLOAT_MATRIX_ENV } }
  input { name { past } type { FLOAT_MATRIX_ENV } }
  output { name { output } type { FLOAT_MATRIX_ENV } }

  defstate {
    name { alpha }
    type { float }
    default { 1.0 }
    desc { Leak value for error-recovery. }
  }

  protected { float leak; }
  setup { leak = float(double(alpha)); }
  
  // This star code uses "wraparound" to represent negative values
  // using unsigned char's. Suppose unsigned char a = 3 and unsigned
  // char b = 4. Then unsigned char (a-b) == 255. BUT, (a-b) + b == 3,
  // which is what we need. Note that the quant() function does not
  // do thresholding for negative floating-point values.
  // The '128' is so this star's output can be compressed with the DCT.
  // Without the '128', the values +1 and -1 are represented as 1 and 255.
  // The DCT can't compress very well then.
  inline method {
    name { quant }
    access { protected }
    type { "unsigned char" }
    arglist { "(float inp, float pst)" }
    code {
      return((unsigned char) (inp + 0.5 + leak * (128.0 - pst)));
    }	
  }

  method {
    name { inputsOk }
    type { "int" }
    access { private }
    arglist { "(const FloatMatrix& one, const FloatMatrix& two)" }
    code {
      int retval = (one.numRows() == two.numRows());
      retval &= (one.numCols() == two.numCols());
      return(retval);
    }
  }

  go {
    // Read data from input.
    Envelope curEnvp, pastEnvp;
    (input%0).getMessage(curEnvp);
    (past%0).getMessage(pastEnvp);
    const FloatMatrix& inImage = *(const FloatMatrix *) curEnvp.myData();
    const FloatMatrix& pastImage = *(const FloatMatrix *) pastEnvp.myData();

    if (pastEnvp.empty()) {
      output%0 << curEnvp;
      return;
    }
    
    if(!inputsOk(inImage, pastImage)) {
      Error::abortRun(*this, "Problem with input images.");
      return;
    }

    int width = inImage.numCols();
    int height = inImage.numRows();

    // Handle real valid inputs...
    LOG_NEW; FloatMatrix& diffImage = *(new FloatMatrix(height,width));
    for(int travel = width*height - 1; travel >= 0; travel--) {
      diffImage.entry(travel) = 
	quant(inImage.entry(travel), pastImage.entry(travel));
    }

    output%0 << diffImage;
  }
} // end defstar { Dpcm }

defstar {
  name { DPCMImageInv }
  domain { SDF }
  version { @(#)SDFDPCMImageInv.pl	1.17 01 Oct 1996 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
This star inverts differential pulse code modulation of an image.
If the "past" input is not a GrayImage or has size 0, pass the "diff"
directly to the "output". Otherwise, add the "past" to the "diff"
(with leakage factor alpha) and send the result to "output".
  }
	htmldoc {
<a name="image decompression"></a>
<a name="image DPCM, inverse"></a>
<a name="DPCM, inverse"></a>
<a name="inverse DPCM"></a>
<a name="decompression, image"></a>
  }
  seealso { DPCMImage }

  hinclude { "Matrix.h", "Error.h" }

//////// I/O AND STATES.
  input { name { diff } type { FLOAT_MATRIX_ENV } }
  input { name { past } type { FLOAT_MATRIX_ENV } }
  output { name { output } type { FLOAT_MATRIX_ENV } }

  defstate {
    name { alpha }
    type { float }
    default { 1.0 }
    desc { Leak value to aid error recovery. }
  }

  protected { float leak; }
  setup { leak = float(double(alpha)); }

  inline method {
    name { quant }
    type { "unsigned char" }
    access { protected }
    arglist { "(const float dif, const float prv)" }
    code {
      return ((unsigned char) (dif + 0.5 + leak * (prv-128.0)));
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
    Envelope diffEnvp, pastEnvp;
    (diff%0).getMessage(diffEnvp);
    (past%0).getMessage(pastEnvp);
    const FloatMatrix& diffImage = *(const FloatMatrix *) diffEnvp.myData();
    const FloatMatrix& pastImage = *(const FloatMatrix *) pastEnvp.myData();

    // Resynchronize if 'past' of wrong type.
    if (pastEnvp.empty()) {
      output%0 << diffEnvp;
      return;
    }
    
    if(!inputsOk(diffImage, pastImage)) {
      Error::abortRun(*this, "Problem with input images.");
      return;
    }

    int width = diffImage.numCols();
    int height = diffImage.numRows();

    LOG_NEW; FloatMatrix& outImage = *(new FloatMatrix(height,width));

    for(int travel = width*height - 1; travel >= 0; travel--) {
      outImage.entry(travel) = 
	quant(diffImage.entry(travel), pastImage.entry(travel));
    }

    // Send the outputs on their way.
    output%0 << outImage;
  }
} // end defstar { DpcmInv }

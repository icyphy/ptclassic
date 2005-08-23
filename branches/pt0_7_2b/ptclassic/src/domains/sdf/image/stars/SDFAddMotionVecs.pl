defstar {
  name { AddMotionVecs }
  domain { SDF }
  version { @(#)SDFAddMotionVecs.pl	1.9 3/7/96 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
Over each block in the input image, superimpose an arrow indicating
the size and direction of the corresponding motion vector.
  } 

  hinclude { "Matrix.h", "Error.h" }

  //////// I/O AND STATES.
  input { name { inimage } type { FLOAT_MATRIX_ENV } }
  input { name { inmvHorz } type { FLOAT_MATRIX_ENV } }
  input { name { inmvVert } type { FLOAT_MATRIX_ENV } }

  output { name { outimage } type { FLOAT_MATRIX_ENV } }

  defstate {
    name { BlockSize }
    type { int }
    default { 8 }
    desc { Size of blocks on which doing the motion comp. }
  }

  method {
    name { inputsOk }
    type { "int" }
    access { private }
    arglist { "(const FloatMatrix& gg, const FloatMatrix& mvH, const FloatMatrix& mvV)" }
    code {
      int bs = int(BlockSize);
      int retval = (gg.numCols() == mvH.numCols() * bs);
      retval &= (gg.numRows() == mvH.numRows() * bs);
      retval &= (gg.numCols() == mvV.numCols() * bs);
      retval &= (gg.numRows() == mvV.numRows() * bs);

      return retval;
    }
  }

  go {
    // Read data from input.
    Envelope imgEnvp, mvHorzEnvp, mvVertEnvp;
    (inimage%0).getMessage(imgEnvp);
    (inmvHorz%0).getMessage(mvHorzEnvp);
    (inmvVert%0).getMessage(mvVertEnvp);
    const FloatMatrix& inImage = *(const FloatMatrix *)imgEnvp.myData();
    const FloatMatrix& mvHorzImg = *(const FloatMatrix *)mvHorzEnvp.myData();
    const FloatMatrix& mvVertImg = *(const FloatMatrix *)mvVertEnvp.myData();

    // Handle null mvec fields.
    if ( mvHorzImg.numRows()*mvHorzImg.numCols() == 0  ||
	 mvVertImg.numRows()*mvVertImg.numCols() == 0  ) {
      outimage%0 << imgEnvp;
      return;
    }

    // Are inputs ok?
    if (!inputsOk(inImage, mvHorzImg, mvVertImg)) {
      Error::abortRun(*this, "Problem with input images.");
      return;
    }

    // We have a non-NULL mvec field. Add the arrows.
    const int bSize = int(BlockSize);

    const int width = inImage.numCols();
    const int height = inImage.numRows();
    FloatMatrix& outImage = *(new FloatMatrix(inImage));

    int i, j, k;

    for(i = 0; i < height; i += bSize) {
      for(j = 0; j < width; j += bSize) {
	float ii = i + bSize/2;
	float jj = j + bSize/2;
	unsigned char color;
	
	// Set color of arrow.
	int dcVal = int( outImage.entry(int(ii)*width+int(jj)) +
	            outImage.entry((1+int(ii))*width+int(jj)) +
		    outImage.entry(int(ii)*width+int(jj)+1) +
		    outImage.entry((1+int(ii))*width+int(jj)+1) );
	if (dcVal > 768) { color = (unsigned char) 0; }
	else { color = (unsigned char) 255; }
	
	// Square in center.
	outImage.entry(int(ii)*width+int(jj)) =
	  outImage.entry((1+int(ii))*width+int(jj)) =
	  outImage.entry(int(ii)*width+int(jj)+1) =
	  outImage.entry((1+int(ii))*width+int(jj)+1) = color;
	
	// Arrow
	float vertInc = mvVertImg.entry(i*width/(bSize*bSize)+j/bSize) 
	                / (2.0*bSize);
	float horzInc = mvHorzImg.entry(i*width/(bSize*bSize)+j/bSize) 
	                / (2.0*bSize);
	for(k = 0; k < bSize; k++) {
	  outImage.entry(int(ii)*width + int(jj)) = color;
	  ii += vertInc;	jj += horzInc;
	}
      }
    } // end for(each block)

    // Send the output on its way.
    outimage%0 << outImage;
	}
} // end defstar { AddMvecs }

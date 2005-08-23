defstar {
  name { MotionCmp }
  domain { SDF }
  version { @(#)SDFMotionCmp.pl	1.30 09/10/99 }
  author { Paul Haskell }
  copyright {
Copyright (c) 1990-1999 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }
  location { SDF image library }
  desc {
If the "past" input is not a Float Matrix (e.g. "dummyMessage"), copy
the "input" image unchanged to the "diffOut" output and send a null
field (zero size matrix) of motion vectors to "mvHorzOut" and 
"mvVertOut" outputs. This should usually happen only on the first
firing of the star.

For all other inputs, perform motion compensation and write the
difference frames and motion vector frames to the corresponding outputs.

This star can be used as a base class to implement slightly different
motion compensation algorithms. For example, synchronization techniques
can be added or reduced-search motion compensation can be performed.
  }
	htmldoc {
<a name="motion compensation"></a>
<a name="image, motion compensation"></a>
  }

  hinclude { "Matrix.h", "Error.h", <stdio.h> }

  //////// I/O AND STATES.
  input { name { input } type { FLOAT_MATRIX_ENV } }
  input { name { past } type { FLOAT_MATRIX_ENV } }
  output { name { diffOut } type { FLOAT_MATRIX_ENV } }

  output { name { mvHorzOut } type { FLOAT_MATRIX_ENV } }
  output { name { mvVertOut } type { FLOAT_MATRIX_ENV } }

  defstate {
    name { BlockSize }
    type { int }
    default { 8 }
    desc { Size of blocks on which to do motion comp. }
  }

  protected { int bs; }
  setup { bs = int(BlockSize); }

  method {
    name { doMC }
    type { "void" }
    access { private }
    arglist { "(FloatMatrix& diff, const FloatMatrix& cur, const FloatMatrix&  prev, FloatMatrix& horzImg, FloatMatrix& vertImg, const int width, const int height)" }
    code {
      int count = 0;
      char horz, vert;
      for(int ii = 0; ii < height; ii += bs) {
	for(int jj = 0; jj < width; jj += bs) {
	  int xvec, yvec;
	  FindMatch(cur, prev, ii, jj, xvec,
		    yvec, width, height);
	  DoOneBlock(horz, vert, diff, cur,
		     prev, ii, jj, xvec, yvec,
		     width);
	  horzImg.entry(count) = horz;
	  vertImg.entry(count) = vert;
	  count++;
	}
      } // end middle rows
    } // end code{}
  } // end doMC()


  virtual method {
    name { FindMatch }
    type { "void" }
    arglist { "(const FloatMatrix& cur, const FloatMatrix& prev, const int ii, const int jj, int& xvec, int& yvec, const int width, const int height)" }
    access { protected }
    code {
      // If we're near the border, don't do motion comp.
      if ((ii == 0) || (jj == 0) ||
	  (ii == height-bs) ||
	  (jj == width-bs)) {
	xvec = yvec = 0;
	return;
      }
      
      int bs2 = 2 * bs;
      register int tmp1, tmp2, tmp3;
      LOG_NEW; int *diffArr = new int[bs2*bs2];

      // Set difference values for each offset
      for(int deli = 0; deli < bs2; deli++) {
	for(int delj = 0; delj < bs2; delj++) {
	  tmp3 = deli*bs2 + delj;
	  diffArr[tmp3] = 0;
	  for(int i = 0; i < bs; i++) {
	    tmp1 = (ii+i)*width + jj;
	    tmp2 = (ii+i+deli-bs)*width +
	      jj+delj-bs;
	    for(int j = 0; j < bs; j++) {
	      diffArr[tmp3]+=abs(int(cur.entry(tmp1+j)-prev.entry(tmp2+j)));
	    }
	  }
	}
      }

      // Find min difference
      int mini = bs;
      int minj = bs;
      for(int i = 0; i < bs2; i++) {
	for(int j = 0; j < bs2; j++) {
	  if (diffArr[bs2*i+j] < diffArr[bs2*mini+minj]) {
	    mini = i; minj = j;
	  }
	}
      }
      yvec = mini - bs; xvec = minj - bs;
      LOG_DEL; delete [] diffArr;
    }
  } // end FindMatch{}


  virtual method {
    name { DoOneBlock }
    access { protected }
    type { "void" }
    arglist { "(char& horz, char& vert, FloatMatrix& diff, const FloatMatrix& cur, const FloatMatrix& prev, const int ii, const int jj, const int xvec, const int yvec, const int width)" }
    code {
      // Set diff frame and mvects.
      int tmp1, tmp2;
      for(int i = 0; i < bs; i++) {
	tmp1 = (ii+i+yvec)*width + jj+xvec;
	tmp2 = (ii+i)*width + jj;
	for(int j = 0; j < bs; j++) {
	  diff.entry(tmp2+j) = 
	          quant(int(cur.entry(tmp2+j)),int(prev.entry(tmp1+j)));
	}
      }	
		
	// NOTE THE -1's!! These are so the motion vector points FROM the past
	// block TO the current block, rather than the other way around!
	horz = char(-1*xvec);
	vert = char(-1*yvec);
      }
    } // end DoOneBlock{}

    // DIVIDE DIFFERENCE BY 2 TO FIT INTO 8 BITS!!
    // ADD 128 SO NEGATIVE NUMBERS DON'T HAVE TO WRAP AROUND!!
    inline virtual method {
      name { quant }
      access { protected }
      type { "unsigned char" }
      arglist { "(const int i1, const int i2)"}
      code {
	int f = (i1 - i2) / 2;
	return ((unsigned char) (f + 128));
      }
    }

  method {
    name { inputsOk }
    access { private }
    type { "int" }
    arglist { "(const FloatMatrix& one, const FloatMatrix& two, char* buf)" }
    code {
      if (one.numCols() != two.numCols()) {
	sprintf(buf, "Widths differ: %d %d", one.numCols(), two.numCols());
	return FALSE;
      }
      if (one.numRows() != two.numRows()) {
	sprintf(buf, "Heights differ: %d %d",one.numRows(),two.numRows());
	return FALSE;
      }
      if (one.numCols() != (bs * (one.numCols() / bs))) {
	sprintf(buf,"Blocksize-width prob: %d %d",bs,one.numCols());
	return FALSE;
      }
      if (one.numRows() != (bs * (one.numRows() / bs))) {
	sprintf(buf,"Blocksize-height prob: %d %d",bs,one.numRows());
	return FALSE;
      }
      return TRUE;
    }
  } // end inputsOk()

  go {
    // Read inputs.
    Envelope pastEnvp, curEnvp;
    (past%0).getMessage(pastEnvp);
    (input%0).getMessage(curEnvp);
    const FloatMatrix& prvImage = *(const FloatMatrix *)pastEnvp.myData();
    const FloatMatrix& inImage  = *(const FloatMatrix *)curEnvp.myData();

    // Initialize if this is the first input image.
    if (pastEnvp.empty()) {
      diffOut%0 << curEnvp;

      LOG_NEW; 
      FloatMatrix& empty = *(new FloatMatrix);
      mvHorzOut%0 << empty;
      mvVertOut%0 << empty;

      return;
    }

    char buf[512];
    if (!inputsOk(inImage, prvImage, buf)) {
      Error::abortRun(*this, buf);
      return;
    }

    // If we reached here, we already have read one image.
    // Create difference and mvect images.
    int height = inImage.numRows();
    int width  = inImage.numCols();
    LOG_NEW;
    FloatMatrix& outImage = *(new FloatMatrix(height,width));

    LOG_NEW;
    FloatMatrix& mvHorzImg = *(new FloatMatrix(height/bs,width/bs));
    LOG_NEW;
    FloatMatrix& mvVertImg = *(new FloatMatrix(height/bs,width/bs));

    ////// Do the motion compensation.
    doMC(outImage, inImage, prvImage, mvHorzImg, mvVertImg, width, height);

    // Send the outputs on their way.
    diffOut%0 << outImage;
    mvHorzOut%0 << mvHorzImg;
    mvVertOut%0 << mvVertImg;
  } // end go{}
} // end defstar { MotionCmp }

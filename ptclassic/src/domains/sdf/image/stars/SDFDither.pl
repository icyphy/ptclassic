defstar {
    name {Dither}
    domain {SDF}
    desc {
Do digital halftoning (dither) of input image for monochrome printing.
Input image should be in a float matrix.
The possible dither methods are: Err-Diffusion, Clustered, Dispersed, Own.
If you specify "Own", then you can use your own dither mask.
    }
    version { $Id$ }
    author { Bilung Lee }
    copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { SDF image library }
    input {
        name {input}
        type {FLOAT_MATRIX_ENV}
    }
    output {
        name {output}
        type {FLOAT_MATRIX_ENV}
    }
    defstate {
	name {ditherType}
	type {string}
	default {"Err-Diffusion"}
	desc {Name of the detector to be used.}
    }
    defstate {
	name {ownMask}
	type {floatarray}
        default {""}
	desc {Dither mask to be used when specify "Own" in ditherType.}
    }
    defstate {
	name {rowsOwnMask}
	type {int}
        default {"4"}
	desc {Number of rows for ownMask.}
    }
    defstate {
	name {colsOwnMask}
	type {int}
        default {"4"}
	desc {Number of columns for ownMask.}
    }
    protected {
        double *mask;
        int rowsMask, colsMask;
    }
    code {
	extern "C" {
	    extern int strcasecmp(const char*,const char*);
	}
    }
    ccinclude {"Matrix.h"}
    setup {
	const char *dt = ditherType;

	/*IF*/ if ( strcasecmp( dt, "Err-Diffusion")==0 ) {
            /* It doesn't need dither mask for error diffusion. */
	} else if ( strcasecmp( dt, "Clustered")==0 ) {
            /* Dither mask for clustered dither. */
            rowsMask=4; colsMask=4;
            mask = new double[rowsMask*colsMask];
            mask[0] = 75; mask[1] = 30; mask[2] = 45; mask[3] =105;
            mask[4] =150; mask[5] = 15; mask[6] =225; mask[7] =240;
            mask[8] =135; mask[9] = 90; mask[10]=210; mask[11]=180;
            mask[12]=120; mask[13]= 60; mask[14]=195; mask[15]=165;
	} else if ( strcasecmp( dt, "Dispersed")==0 ) {
            /* Dither mask for dispersed dither.  */
            rowsMask=4; colsMask=4;
            mask = new double[rowsMask*colsMask];
            mask[0] = 15; mask[1] =135; mask[2] = 45; mask[3] =165;
            mask[4] =195; mask[5] = 75; mask[6] =225; mask[7] =105;
            mask[8] = 60; mask[9] =180; mask[10]= 30; mask[11]=150;
            mask[12]=240; mask[13]=120; mask[14]=210; mask[15]= 90;
	} else if ( strcasecmp( dt, "Own")==0 ) {
            /* Use dither mask specified by user. */
            int size = ownMask.size();
            rowsMask=int(rowsOwnMask); colsMask=int(colsOwnMask);
            //  check if ownMask doesn't have enough elements.
            if ( size != rowsMask*colsMask ) {
	       Error::abortRun(*this,"The number of elements in 'ownMask' ",
			       "does't match the specified dimension.");
	       return;
            }
            mask = new double[rowsMask*colsMask];
            mask = (double *)ownMask;
	} else {
	    Error::abortRun(*this, ": Unknown dither type.");
	    return;
	}
    }
    go {
        int i,j;

        Envelope pkt;
        (input%0).getMessage(pkt);
        const FloatMatrix& matrix = *(const FloatMatrix*)pkt.myData();

        if(pkt.empty()) {
          // input empty, send out a zero matrix
          output%0 << double(0);
        }
        else {
          // valid input matrix
          int height = matrix.numRows();
          int width  = matrix.numCols();

          FloatMatrix& result = *(new FloatMatrix(matrix));

          const char *dt = ditherType;

	  if ( strcasecmp( dt, "Err-Diffusion")==0 ) {
	    /* Do the error diffusion */
	    double newValue, oldValue, error, value;

	    for (i=0; i<height; i++) {
	      for (j=0; j<width; j++) {
		oldValue = result[i][j];
		/* The threshold is at 128. 0 for black, and 255 for white. */
		if (oldValue < 128) {
		  newValue = 0;
		  result[i][j] = newValue;
		} else {
		  newValue = 255;
		  result[i][j] = newValue;
		}

		/* Propagate error to neighbor dot */
		error = oldValue - newValue;
		if ((j+1)<width) {
		  value = result[i][j+1]+error*7/16;
		  result[i][j+1]=bound(value);
		}
		if ((i+1)<height && (j-1)>=0) {
		  value = result[i+1][j-1]+error*3/16;
		  result[i+1][j-1]=bound(value);
		} 
		if ((i+1)<height) {
		  value = result[i+1][j]+error*5/16;
		  result[i+1][j]=bound(value);
		}
		if ((i+1)<height && (j+1)<width) {
		  value = result[i+1][j+1]+error*1/16;
		  result[i+1][j+1]=bound(value);
		}
	      }
            }
	  } else {
	    /* Do the clustered dither or dispersed dither 
               depending on the mask */
	    for (i=0; i<height; i++) {
	      for (j=0; j<width; j++) {
		/* compare with threshold in the mask,then determine B/W */
		if (255-result[i][j] >= 
                    mask[(i%rowsMask)*colsMask+(j%colsMask)])
		  result[i][j] = 0;
		else 
		  result[i][j] = 255;
	      }
	    } 
	  }
          output%0 << result;
	} // end of valid input matrix.
    } 
    method {
        name { bound }
        access { private }
        arglist { "(double value)" }
        type { double }
        code {
          /* Bound the value of the matrix into 0~255. */
	  if (value<0) value = 0;
	  if (value>255) value = 255;
	  return value;
        }
    }
}

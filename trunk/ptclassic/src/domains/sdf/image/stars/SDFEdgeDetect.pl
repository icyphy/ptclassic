defstar {
    name {EdgeDetect}
    domain {SDF}
    desc {
Detect edges in input image. Input image should be in a float matrix.
The possible detectors are: Sobel, Roberts, Prewitt, Frei-Chen,
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
	name {threshold}
	type {float}
	default {"50"}
	desc {Threshold to be used to determine edges.}
    }
    defstate {
	name {detectorName}
	type {string}
	default {"Sobel"}
	desc {Name of the detector to be used.}
    }
    protected {
        double hx[3][3], hy[3][3];
    }
    code {
	extern "C" {
	    extern int strcasecmp(const char*,const char*);
	}
    }
    ccinclude {"Matrix.h"}
    setup {
	const char *dn = detectorName;

	/*IF*/ if ( strcasecmp( dn, "Sobel")==0 ) {
            /* hx and hy for Sobel method */
            hx[0][0]= 1.0/4; hx[0][1]=     0; hx[0][2]=-1.0/4;
            hx[1][0]= 2.0/4; hx[1][1]=     0; hx[1][2]=-2.0/4;
            hx[2][0]= 1.0/4; hx[2][1]=     0; hx[2][2]=-1.0/4;
            hy[0][0]=-1.0/4; hy[0][1]=-2.0/4; hy[0][2]=-1.0/4;
            hy[1][0]=     0; hy[1][1]=     0; hy[1][2]=     0;
            hy[2][0]= 1.0/4; hy[2][1]= 2.0/4; hy[2][2]= 1.0/4;
	} else if ( strcasecmp( dn, "Roberts")==0 ) {
            /* hx and hy for Roberts method */
            hx[0][0]=   0;   hx[0][1]=  0;    hx[0][2]=-1.0;
            hx[1][0]=   0;   hx[1][1]=1.0;    hx[1][2]=   0;
            hx[2][0]=   0;   hx[2][1]=  0;    hx[2][2]=   0;
            hy[0][0]=-1.0;   hy[0][1]=  0;    hy[0][2]=   0;
            hy[1][0]=   0;   hy[1][1]=1.0;    hy[1][2]=   0;
            hy[2][0]=   0;   hy[2][1]=  0;    hy[2][2]=   0;
	} else if ( strcasecmp( dn, "Prewitt")==0 ) {
            /* hx and hy for Prewitt method */
            hx[0][0]= 1.0/3; hx[0][1]=     0; hx[0][2]=-1.0/3;
            hx[1][0]= 1.0/3; hx[1][1]=     0; hx[1][2]=-1.0/3;
            hx[2][0]= 1.0/3; hx[2][1]=     0; hx[2][2]=-1.0/3;
            hy[0][0]=-1.0/3; hy[0][1]=-1.0/3; hy[0][2]=-1.0/3;
            hy[1][0]=     0; hy[1][1]=     0; hy[1][2]=     0;
            hy[2][0]= 1.0/3; hy[2][1]= 1.0/3; hy[2][2]= 1.0/3;
	} else if ( strcasecmp( dn, "Frei-Chen")==0 ) {
            /* hx and hy for Frei-Chen method */
            double d = 2+sqrt(2);
            hx[0][0]=    1.0/d; hx[0][1]=         0; hx[0][2]=    -1.0/d;
            hx[1][0]=sqrt(2)/d; hx[1][1]=         0; hx[1][2]=-sqrt(2)/d;
            hx[2][0]=    1.0/d; hx[2][1]=         0; hx[2][2]=    -1.0/d;
            hy[0][0]=   -1.0/d; hy[0][1]=-sqrt(2)/d; hy[0][2]=    -1.0/d;
            hy[1][0]=        0; hy[1][1]=         0; hy[1][2]=         0;
            hy[2][0]=    1.0/d; hy[2][1]= sqrt(2)/d; hy[2][2]=     1.0/d;
	} else {
	    Error::abortRun(*this, ": Unknown detector name");
	    return;
	}
    }
    go {
        int i;
        int n1,n2,k1,k2;
        double fx, fy;

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

          FloatMatrix& result = *(new FloatMatrix(height,width));
	  for (n2=0; n2<height; n2++) {
	    for (n1=0; n1<width; n1++) {
	      fx = 0;
	      /* Compute matrix(n1,n2) convolve with hx(n1,n2) */
	      for (k2=0; k2<3; k2++) {
		for (k1=0; k1<3; k1++) {
		  if ((n2-(k2-1))>=0  &&  (n1-(k1-1))>=0)
		    fx += hx[k2][k1] * matrix[n2-(k2-1)][n1-(k1-1)];
		}
	      }

	      fy = 0;
	      /* Compute matrix(n1,n2) convolve with hy(n1,n2) */
	      for (k2=0; k2<3; k2++) {
		for (k1=0; k1<3; k1++) {
		  if ((n2-(k2-1))>=0  &&  (n1-(k1-1))>=0)
		    fy += hy[k2][k1] * matrix[n2-(k2-1)][n1-(k1-1)];
		}
	      }

	      /* Compute the enhanced version of the matrix */
	      result[n2][n1] = sqrt(fx*fx+fy*fy);
	    }
	  }

	  /* Now what we have is only the enhanced version of input matrix,
	     so we need to use the threshold to determine edges. */
	  for (i=0; i<width*height; i++) {
	    if (result.entry(i) >= threshold) 
              result.entry(i)=255; /*255 for an edge.*/
	    else 
              result.entry(i)=0;   /* 0 for not an edge. */
	  }
          output%0 << result;
	}
    }  
}

defstar {
	name {FFTCx}
	domain {MDSDF}
	desc {
Compute the 2D discrete Fourier transform of a complex input using the
row-column decomposition with 1D fast Fourier transform (FFT) algorithm.
"rowOrder" (default 8) is the log base 2 of the row transform size.
"colOrder" (default 8) is the log base 2 of the column transform size.
"numRows" (default 256) is the number of rows in the input.
"numCols" (default 256) is the number of columns in the input.
	}
	version { $Id$ }
	author { J. T. Buck, modified for MDSDF by Mike J. Chen }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { MDSDF library }
	explanation {
A number of input samples given by the parameter \fIsize\fR will
be consumed at the input, zero-padded if necessary to make $2 sup order$
samples, and transformed using a fast Fourier transform algorithm.
.Id "FFT, complex"
.Id "fast Fourier transform, complex"
.Id "Fourier transform, fast, complex"
If \fIdirection\fR is 1, then the forward Fourier transform is computed.
If \fIdirection\fR is -1, then the inverse Fourier transform is computed.
.lp
Note a single firing of this star consumes \fIsize\fR inputs
and produces $2 sup order$ outputs.
This must be taken into account when determining for how many iterations
to run a universe.
For example, to compute just one FFT, only one iteration should be run.
.lp
\fBBugs\fR: the routine currently used (from Gabriel) recomputes trig
.Ir "Gabriel"
functions for each term, instead of using a table.
Instead, FFTCx::setup() should compute a table of appropriate size
in order to save time.
This approach does not offer any improvement, obviously, if only
one transform is performed.
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.UH REFERENCES
.ip [1]
Jae S. Lim, \fITwo-Dimensional Signal and Image Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1990.
	}
	input {
		name {input}
		type {COMPLEX_MATRIX}
	}
	output {
		name {output}
		type {COMPLEX_MATRIX}
	}
	defstate {
		name {rowOrColumn}
		type {int}
		default {0}
		desc {Select a row or column FFT, 0 = row, 1 = col.}
	}
	defstate {
		name {order}
		type {int}
		default {8}
		desc {Log base 2 of the transform size.}
	}
	defstate {
		name {size}
		type {int}
		default {256}
		desc {The number of rows or columns in the input.}
	}
	defstate {
		name {direction}
		type {int}
		default {1}
		desc {Equals 1 for forward, or -1 for inverse, transform.}
	}
	protected {
		int fftSize;
	}
// Code for the FFT function
	code {

/*
 * This fft routine is from ~gabriel/src/filters/fft/fft.c;
 * I am unsure of the original source.  The file contains no
 * copyright notice or description.
 * The declaration is changed to the prototype form but the
 * function body is unchanged.  (J. T. Buck)
 */

#define SWAP(a, b) temp=(a); (a)=(b); (b)=temp

/*
 * Replace data by its discrete Fourier transform, if isign is
 * input as 1, or by its inverse discrete Fourier transform, if 
 * "isign" is input as -1.  "data'"is a complex array of length "nn",
 * input as a real array data[0..2*nn-1]. "nn" MUST be an integer
 * power of 2 (this is not checked for!?)
 */

static void fft_rif(ComplexSubMatrix *inMatrix, int nn, int isign)
{
	int	mmax;
	int	m, j, istep, i;
	double	wtemp, theta;
	Complex w, wp, temp;

	j = 0;

        // put into bit-reversed order
	for (i = 0; i < nn; i++) {
		if(j > i) {
			SWAP(inMatrix->entry(j), inMatrix->entry(i));
                }
		m = nn >> 1;
		while (m >= 1 && j >= m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 1;
	while (nn > mmax) {
		istep = mmax*2;
		theta = -3.1415926535897949/(isign*mmax);
		wtemp = sin(0.5*theta);
		wp = Complex(-2.0*wtemp*wtemp, sin(theta));
		w = Complex(1.0,0.0);
		for (m = 0; m < mmax; m++) {
			for (i = m; i < nn; i += istep) {
				j = i + mmax;
				temp = w * inMatrix->entry(j);
				inMatrix->entry(j) = inMatrix->entry(i) - temp;
				inMatrix->entry(i) += temp;
			}
			w  = Complex((wtemp = w.real())*wp.real() - 
	                             w.imag()*wp.imag() + w.real(),
				     w.imag()*wp.real() + wtemp*wp.imag() +
				     w.imag());
		}
		mmax = istep;
	}
}

	}	// end of code block for inclusion in .cc file

	setup {
	  fftSize = 1 << int(order);
	  if (fftSize < int(size)) {
	    Error::abortRun(*this, "2^order must be >= size");
	    return;
	  }
          if(int(rowOrColumn)) { // column FFT
	    input.setMDSDFParams(int(size), 1);
	    output.setMDSDFParams(int(fftSize), 1);
	  }
          else { // row FFT
	    input.setMDSDFParams(1, int(size));
	    output.setMDSDFParams(1, int(fftSize));
	  }
	}
	go {
// get input and output matrices
	  ComplexSubMatrix* data = (ComplexSubMatrix*)(input.getInput());
	  ComplexSubMatrix* result = (ComplexSubMatrix*)(output.getOutput());

// copy input to output, we will work on output buffer
          int i;
          for(i = 0; i < int(size); i++)
            result->entry(i) = data->entry(i);

          // zero fill the result of the output buffer
          for(i = int(size); i < fftSize; i++)
	    result->entry(i) = Complex(0.0);
          
          fft_rif(result, fftSize, int(direction));

	  // generate output data.  If inverse, we must scale the result.
	  if (int(direction) != 1)
            for (i = 0; i < fftSize; i++)
              result->entry(i) /= fftSize;
	  // delete access structures
	  delete data;
	  delete result;
	}
}	

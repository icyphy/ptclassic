ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 8/3/90
 Converted to use preprocessor, 9/26/90

Methods for ComplexFFT: Compute the FFT of the input.

Bugs: the routine currently used (from Gabriel) recomputes trig
functions for each term, instead of using a table.  We should have
ComplexFFT::start() compute a table of appropriate size and save
time.

**************************************************************************/
}
defstar {
	name {ComplexFFT}
	domain {SDF}
	desc {
	"Complex Fast Fourier transform.\n"
	"'order' (default 8) is the log, base 2, of the transform size.\n"
	"'size' (default 256) is the number of samples read (<= 2^order).\n"
	"'direction' (default 1) is 1 for forward, -1 for inverse FFT."
	}
	input {
		name {input}
		type {complex}
	}
	output {
		name {output}
		type {complex}
	}
	defstate {
		name {order}
		type {int}
		default {8}
		desc {""}
	}
	defstate {
		name {size}
		type {int}
		default {256}
		desc {""}
	}
	defstate {
		name {direction}
		type {int}
		default {1}
		desc {""}
	}
	protected {
		float* data;
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

#define SWAP(a, b) tempr=(a); (a)=(b); (b)=tempr

/*
 * Replace data by its discrete Fourier transform, if isign is
 * input as 1, or by its inverse discrete Fourier transform, if 
 * "isign" is input as -1.  "data'"is a complex array of length "nn",
 * input as a real array data[0..2*nn-1]. "nn" MUST be an integer
 * power of 2 (this is not checked for!?)
 */

static void fft_rif(float *data, int nn, int isign)
{
	int	n;
	int	mmax;
	int	m, j, istep, i;
	double	wtemp, wr, wpr, wpi, wi, theta;
	float	tempr, tempi;

	data--;
	n = nn << 1;
	j = 1;

	for (i = 1; i < n; i += 2) {
		if(j > i) {
			SWAP(data[j], data[i]);
			SWAP(data[j+1], data[i+1]);
		}
		m= n >> 1;
		while (m >= 2 && j >m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax = 2;
	while (n > mmax) {
		istep = 2*mmax;
		theta = 6.28318530717959/(isign*mmax);
		wtemp = sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi = sin(theta);
		wr = 1.0;
		wi = 0.0;
		for (m = 1; m < mmax; m += 2) {
			for (i = m; i < n; i += istep) {
				j = i + mmax;
				tempr = wr*data[j] - wi*data[j+1];
				tempi = wr*data[j+1] + wi*data[j];
				data[j] = data[i] - tempr;
				data[j+1] = data[i+1] - tempi;
				data[i] += tempr;
				data[i+1] += tempi;
			}
			wr = (wtemp=wr)*wpr - wi*wpi+wr;
			wi = wi*wpr + wtemp*wpi + wi;
		}
		mmax = istep;
	}
}

	const int defSize = 256;
	const char defOrder[] = "8";
	const char defSizeStr[] = "256";

	}	// end of code block for inclusion in .cc file

	constructor {
		data = 0;
	}
	start {
		fftSize = 1 << order;
		if (fftSize < int(size)) {
			Error::abortRun("ComplexFFT: 2^order must be >= size");
			return;
		}
		delete data;
		data = new float[2*fftSize];
		input.setSDFParams (int(size), int(size)-1);
		output.setSDFParams (fftSize, fftSize-1);
	}
	go {
// load up the array
		float* p = data;
// note: particle at maximum delay is the first one
		for (int i = int(size)-1; i >= 0; i--) {
			Complex t = input%i;
			*p++ = t.real();
			*p++ = t.imag();
		}
		for (i = size; i < fftSize; i++) {
			*p++ = 0.0;
			*p++ = 0.0;
		}
		fft_rif (data, fftSize, int(direction));
	// generate output data.  If inverse, we must scale the result.
		if (int(direction) != 1)
			for (i = 0; i < 2*fftSize; i++)
				data[i] /= fftSize;
	// write 'em out
		p = data;
		for (i = fftSize-1; i >= 0; i--) {
			output%i << Complex(p[0], p[1]);
			p += 2;
		}
	}
}	

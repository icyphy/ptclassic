defstar {
	name {FFTCx}
	domain {SDF}
	desc {
Compute the discrete Fourier transform of a complex input using the
fast Fourier transform (FFT) algorithm.  The star reads "size" (default
256) complex samples, zero pads the data if necessary, and then takes an
FFT of length 2 ^ "order", where "size" <= 2 ^ "order".  (The default
value of "order" is 8.)  The parameter "direction" (default 1) is 1 for
the forward and -1 for the inverse FFT.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
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
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
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
		desc {Log base 2 of the transform size.}
	}
	defstate {
		name {size}
		type {int}
		default {256}
		desc {Number of input samples to read.}
	}
	defstate {
		name {direction}
		type {int}
		default {1}
		desc { Equals 1 for forward, or -1 for inverse, transform. }
	}
	protected {
		double* data;
		int fftSize;
	}
	constructor {
		data = 0;
	}
	destructor {
		LOG_DEL; delete [] data;
	}
	ccinclude { "PTDSPfft_rif.h" }
	setup {
		fftSize = 1 << int(order);
		if (fftSize < int(size)) {
			Error::abortRun(*this, "2^order must be >= size");
			return;
		}
		LOG_DEL; delete [] data;
		LOG_NEW; data = new double[2*fftSize];
		input.setSDFParams(int(size), int(size)-1);
		output.setSDFParams(fftSize, fftSize-1);
	}
	go {
		// load up the array
		double* p = data;

		// note: particle at maximum delay is the first one
		for (int i = int(size)-1; i >= 0; i--) {
			Complex t = Complex(input%i);
			*p++ = t.real();
			*p++ = t.imag();
		}
		for (i = size; i < fftSize; i++) {
			*p++ = 0.0;
			*p++ = 0.0;
		}
		PTDSPfft_rif(data, fftSize, int(direction));
		// generate output data.  If inverse, we scale the result.
		if (int(direction) != 1)
			for (i = 0; i < 2*fftSize; i++)
				data[i] /= fftSize;

		// send the data through the output port
		p = data;
		for (i = fftSize-1; i >= 0; i--) {
			output%i << Complex(p[0], p[1]);
			p += 2;
		}
	}
}

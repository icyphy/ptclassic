defstar {
	name {DTFT}
	domain {SDF}
	desc {
Compute the discrete-time Fourier transform (DTFT)
at frequency points specified on the "omega" input.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
This star computes arbitrary samples of the discrete-time Fourier
transform (DTFT) of a finite length sequence [1].
.Id "discrete time Fourier transform"
.Id "Fourier transform, discrete time"
.Ir "spectral estimation"
The \fIsignal\fR input is the signal to be transformed.
The number of input samples consumed is given by \fIlength\fR.
Letting the input samples be written as $a(0), ~ ... ~ ,~ a(L-1)$, where
$L$ is the \fIlength\fR, the output is
.EQ
A ( j omega ) ~=~
{ sum from k=0 to L-1 a(k) e sup {- j omega k T} }
.EN
where $T$ is the time between samples (\fItimeBetweenSamples\fR).
The number of samples produced at the output is determined
by the \fInumberOfSamples\fR parameter.
The values of $omega$ at which samples are taken are provided
by the \fIomega\fR input.
Hence, any frequency range or ranges can be examined at any desired
resolution, and samples need not be taken at uniform intervals.
Note that $omega ~=~ 2 pi / T$ is the sampling frequency.
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	seealso { FFTCx }
	input {
		name {signal}
		type {complex}
		desc { Signal to be transformed. }
	}
	input {
		name {omega}
		type {float}
		desc { Frequency values at which to sample the transform. }
	}
	output {
		name {dtft}
		type {complex}
		desc { The samples of the transform. }
	}
	defstate {
		name {length}
		type {int}
		default {8}
		desc {The length of the input signal.}
	}
	defstate {
		name {numberOfSamples}
		type {int}
		default {128}
		desc {The number of samples of the transform to output.}
	}
	defstate {
		name {timeBetweenSamples}
		type {float}
		default {1.0}
		desc {The time between input samples (T).}
	}
	ccinclude { "ComplexSubset.h" }
	setup {
	    signal.setSDFParams (int(length), int(length) - 1);
	    dtft.setSDFParams
		(int(numberOfSamples), int(numberOfSamples) - 1);
	    omega.setSDFParams
		(int(numberOfSamples), int(numberOfSamples) - 1);
	}
	go {
	    for (int n = int(numberOfSamples)-1; n>=0; n--) {
		Complex sum = Complex(0.0, 0.0);
		for (int i = 0;i < int(length); i++) {
		    sum += Complex(signal%(int(length)-i-1))
			* Complex(
			    cos(double(omega%n)*i*double(timeBetweenSamples)),
			    -sin(double(omega%n)*i*double(timeBetweenSamples)));
		}

		dtft%n << sum;
	    }
	}
}

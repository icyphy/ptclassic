ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/28/90

This star computes arbitrary samples of the discrete-time Fourier
transform of a finite length sequence.
Let it be written $sigma sup 2$.
The \fIsignal\fR input is the signal to be transformed.
Let these be written $a(1), ~ ... ~ ,~ a(N)$, where
$N$ is the \fIlength\fR.
Then the output is
.EQ
A ( j omega ) ~=~
{ sum from k=0 to N a(k) e sup {- j omega k T} }
.EN
where $T$ is the time between samples.
The number of samples produced at the output is determined
by the \fInumberOfSamples\fR parameter.
The values of $omega$ at which samples are taken are provided
by the \fIomega\fR input.  Hence, any frequency range or ranges
can be examined at any desired resolution, and samples need not
be taken at uniform intervals.
Note that $omega ~=~ 2 pi / T$ is
the sampling frequency.

SEE ALSO:
ComplexFFT

**************************************************************************/

}
defstar {
	name {DTFT}
	domain {SDF}
	desc { "Discrete-time Fourier transform" }
	input {
		name {signal}
		type {complex}
		// desc { "signal to be transformed" }
	}
	input {
		name {omega}
		type {float}
		// desc { "Frequency values at which to sample the transform" }
	}
	output {
		name {dtft}
		type {complex}
		// desc { "the samples of the transform" }
	}
	defstate {
		name {length}
		type {int}
		default {8}
		desc {"The length of the input signal"}
	}
	defstate {
		name {numberOfSamples}
		type {int}
		default {128}
		desc {"The number of samples of the transform to output"}
	}
	defstate {
		name {timeBetweenSamples}
		type {float}
		default {1.0}
		desc {"The time between input samples (T)"}
	}
	ccinclude { "ComplexSubset.h" }
	start {
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

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
	htmldoc {
This star computes arbitrary samples of the discrete-time Fourier
transform (DTFT) of a finite length sequence [1].
<a name="discrete time Fourier transform"></a>
<a name="Fourier transform, discrete time"></a>
<a name="spectral estimation"></a>
The <i>signal</i> input is the signal to be transformed.
The number of input samples consumed is given by <i>length</i>.
Letting the input samples be written as <i>a</i>(0)<i>,   ...   ,  a</i>(<i>L-</i>1), where
<i>L</i> is the <i>length</i>, the output is
<pre>
             L-1
             ---
A(j omega) = \   a(k) e<sup>-j omega kT</sup>
             /
             ---
             k=0  
</pre>
where <i>T</i> is the time between samples (<i>timeBetweenSamples</i>).
The number of samples produced at the output is determined
by the <i>numberOfSamples</i> parameter.
The values of <i>omega</i> at which samples are taken are provided
by the <i>omega</i> input.
Hence, any frequency range or ranges can be examined at any desired
resolution, and samples need not be taken at uniform intervals.
Note that <i>omega  </i>= 2<i> pi / T</i> is the sampling frequency.
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
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

defstar {
	name {Sinc}
	domain {SDF}
	desc {
This star computes the sinc of its input given in radians.
The sinc function is defined as sin(x)/x, with value 1.0 when x = 0.
	}
        version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
The discrete-time Fourier transform (DTFT) of a sampled sinc function is 
an ideal lowpass filter [1-2].
Modulating a sampled sinc function by a cosine function gives an
ideal bandpass signal.
This star defines the sinc function \fIwithout\fR using $pi$,
as is the convention in [2-3].
.ID "Bracewell, R. N."
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.ID "Willsky, A."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
.ip [2]
A. V. Oppenheim and A. Willsky, \fISignals and Systems\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1983.
.ip [3]
R. N. Bracewell, \fIThe Fourier Transform and Its Applications\fR,
McGraw-Hill: New York, 1986.
	}
	seealso { Dirichlet RaisedCos }
	input {
		name{input}
		type{float}
		desc{The input x to the sinc function.}
	}
	output {
		name{output}
		type{float}
		desc{The output of the sinc function.}
	}
	ccinclude { <math.h> }
	go {
		const double DELTA = 1.0e-9;	// Approximately zero
		double x = input%0;
		double sincValue;

		if ( ( -DELTA < x ) && ( x < DELTA ) )
		  sincValue = 1.0;
		else
		  sincValue = sin(x) / x;

		output%0 << sincValue;
	}
}

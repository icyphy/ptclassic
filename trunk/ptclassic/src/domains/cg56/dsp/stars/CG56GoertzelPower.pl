defstar {
	name { GoertzelPower }
	domain { CG56 }
	derived { GoertzelBase }
	version { $Id$ }
	desc {
First-order recursive computation of the power of the kth coefficient of
an N-point DFT using Goertzel's algorithm.
This form is used in touchtone decoding.
	}
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	explanation {
.lp
This one-pole IIR filter has a transfer function of
.EQ
{1} over
{1 ~-~ {W sub N} sup {-k} z sup -1} ~.
.EN
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	output {
		name{ output }
		type{ fix }
	}
	
	// Inherit setup from GoertzelBase

	codeblock(result) {
; Compute the power of last filter output zz* = Re(z)^2 + Im(z)^2,
; where x0 = Re(z) and b = Im(z)
	mpyr	x0,x0,a		b,y0
	macr	y0,y0,a
	move	a,$ref(output)
	}
	go {
		// Run the Goertzel second-order IIR filter
		// real coeff. stored in x0, imaginary coeff. stored in b
		CG56GoertzelBase::go();

		// Compute the (real-valued) power of last (complex) sample
		addCode(result);
	}

	exectime {
		return (3 + CG56GoertzelBase::myExecTime());
	}
}


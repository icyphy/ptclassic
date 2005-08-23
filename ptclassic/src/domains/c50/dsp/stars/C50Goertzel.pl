defstar {
	name { Goertzel }
	domain { C50 }
	derived	{ GoertzelBase }
	version { $Id$}
	desc {
First-order recursive computation of the kth coefficient of an
N-point DFT using Goertzel's algorithm.
	}
	author { Luis Gutierrez }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	explanation {
.lp
This one-pole IIR filter has a transfer function of
.EQ
{1} over
{1 ~-~ {W sub N} sup {-k} z sup -1 } ~.
.EN
.ID "Goertzel's algorithm"
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	output {
		name{output}
		type{COMPLEX}
	}

	// Inherit the setup method from the GoertzelBase star

	codeblock(result) {
	lar	ar3,#$addr(output)
	sach	*+,2		; output real part
	pac			; acc = p
	sach	*,1		; output imaginary part
	}

	go {
		// Discard all but the last sample
		C50GoertzelBase::go();

		// Output the complex result
		addCode(result);
	}

	exectime {
		return (4 + C50GoertzelBase::myExecTime());
	}
}






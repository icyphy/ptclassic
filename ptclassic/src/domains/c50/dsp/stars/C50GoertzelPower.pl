defstar {
	name { GoertzelPower }
	domain { C50 }
	derived { GoertzelBase }
	version { @(#)C50GoertzelPower.pl	1.10	3/27/96 }
	desc {
First-order recursive computation of the power of the kth coefficient of
an N-point DFT using Goertzel's algorithm.
This form is used in touchtone decoding.
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
	lar	ar3,#0013h
	sach	*+,2
	pac
	sach	*,1
	zap	
	sqra	*-
	sqra	*,ar6
	bcndd	$starSymbol(cfe),UNC
	lar	ar6,#$addr(output)
	sach	*,1
	}
	go {
		C50GoertzelBase::go();

		// Compute the (real-valued) power of last (complex) sample
		addCode(result);
	}

	exectime {
		return (3 + C50GoertzelBase::myExecTime());
	}
}


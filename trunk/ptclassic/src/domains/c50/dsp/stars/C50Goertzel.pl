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
	htmldoc {
<p>
This one-pole IIR filter has a transfer function of
<pre>
{1} over
{1 ~-~ {W sub N} sup {-k} z sup -1 } ~.
</pre>
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
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
	bcndd	$starSymbol(cfe),UNC
	pac			; acc = p
	sach	*,1		; output imaginary part
	}

	go {
		// Discard all but the last sample
		C50GoertzelBase::go();

		// Output the complex result
		addCode(result);
		addCode(C50GoertzelBase::coeffs);
		
	}

	exectime {
		return (4 + C50GoertzelBase::myExecTime());
	}
}






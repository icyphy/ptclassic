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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
<a name="goertzel"> </a>
<p>
This two-pole, one-zero IIR filter has a transfer function of
<pre>
(1 &#43 z<sup>-1</sup>)
 ----------------------------------
(1 - 2&#183(cos( 2&#183<i>pi</i>&#183k/N ))&#183z<sup>-1</sup> &#43 z<sup>-2</sup>)
</pre>
This filter is a biquad filter with
<p>
<ul>
<li> n<sub>0</sub> = 1
<li> n<sub>1</sub> = -(W<sub>N</sub>) <sup>k</sup> = exp(j &#183 2 &#183 <i>pi</i> &#183 k / N )
<li> n<sub>2</sub> = 0
<li> d<sub>1</sub> = - 2 &#183 cos( 2 &#183 <i>pi</i> &#183 k / N )
<li> d<sub>2</sub> = 1
</ul>
<p>
The implementation takes the simpler forms of n<sub>0</sub>, n<sub>2</sub>
and d<sub>2</sub>
into account, and computes n<sub>1</sub> and d<sub>1</sub>
from the parameters k and N.
It is implemented in direct form II.
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






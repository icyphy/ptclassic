defstar {
	name { GoertzelPower }
	domain { C50 }
	derived { GoertzelBase }
	version { $Id$} 
	desc {
First-order recursive computation of the power of the kth coefficient of
an N-point DFT using Goertzel's algorithm.
This form is used in touchtone decoding.
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
<a name="goertzelPower"> </a>
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
This implementation efficiently implements the biquad section based
on the values of these coefficients, and computes d<sub>1</sub> from the
parameters k and N.
It is implemented in direct form II, and requires three additions,
one multiply,
<h3>References</h3>
<p>[1]
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	output {
		name{ output }
		type{ fix }
	}
	
	// Inherit setup from GoertzelBase

	codeblock(result) {
	lar	ar3,#0014h
	sach	*+,2
	pac
	sach	*,1
	zap	
	sqra	*-
	lar	ar6,#$addr(output)
	sqra	*
	bcndd	$starSymbol(cfe),UNC
	lta	*,ar6
	sach	*,1
	}
	go {
		C50GoertzelBase::go();

		// Compute the (real-valued) power of last (complex) sample
		addCode(result);
		addCode(C50GoertzelBase::coeffs);
	}

	exectime {
		return (3 + C50GoertzelBase::myExecTime());
	}
}


defstar {
	name { Goertzel }
	domain { CG56 }
	derived	{ GoertzelBase }
	version { @(#)CG56Goertzel.pl	1.13	06 Oct 1996 }
	desc {
First-order recursive computation of the kth coefficient of an
N-point DFT using Goertzel's algorithm.
	}
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	htmldoc {
<p>
This one-pole IIR filter has a transfer function of
<pre>
    1
-----------
1 - W<sub>N</sub><sup>-k</sup>z<sup>-1</sup>.
</pre>
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	output {
		name{output}
		type{complex}
	}

	// Inherit the setup method from the GoertzelBase star

	codeblock(result) {
	move 	x0,a
	move	ab,$ref(output)
	}

	go {
		// Discard all but the last sample
		// real coeff. stored in x0, imaginary coeff. stored in b
		CG56GoertzelBase::go();

		// Output the complex result
		addCode(result);
	}

	exectime {
		return (2 + CG56GoertzelBase::myExecTime());
	}
}


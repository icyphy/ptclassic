defstar {
	name { Dirichlet }
	domain { CGC }
	desc {
This star computes the normalized Dirichlet kernel (also called the aliased
sinc function):

d(x) = sin(N x / 2) / (N sin(x/2))

The value of the normalized Dirichlet kernel at x = 0 is always 1, and the
normalized Dirichlet kernel oscillates between -1 and +1.
The normalized Dirichlet kernel is periodic in x with a period of either
2*pi when N is odd or 4*pi when N is even.
	}
	version { $Id$ }
	author { Brian Evans }
	copyright {
Copyright (c) 1995-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC dsp library }
	htmldoc {
The Dirichlet kernel (a.k.a. the aliased sinc function) is the
discrete-time Fourier transform (DTFT) of a sampled pulse function.
The parameter <i>N</i> is the length of the pulse [1].
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
        }
	seealso { Sinc }
	input {
		name{ input }
		type{ float }
		desc{ The input x to the Dirichlet kernel. }
	}
	output {
		name{ output }
		type{ float }
		desc{ The output of the Dirichlet kernel. }
	}
	defstate {
		name { N }
		type { int }
		default { 10 }
		desc { Length of the Dirichlet kernel. }
	}
	initCode {
	        addInclude("<math.h>");
	}
	go {
	        addCode(dirichlet);
	}
	codeblock(dirichlet) {
		const double DELTA = 1.0e-9;
		double x = $ref(input);
    		double sinInDenom = sin(x/2);
		double length = (double)$val(N);
		double dirichletValue;

		if ( ( -DELTA < sinInDenom ) && ( sinInDenom < DELTA ) )
		  // Use L'Hopital's Rule when sin(x/2) is approx. 0
		  dirichletValue = cos(length * x / 2) / cos(x / 2);
		else
		  // Otherwise, compute it using the definition
		  dirichletValue = sin(length * x / 2) / (length * sinInDenom);

		$ref(output) = dirichletValue;
	}
}

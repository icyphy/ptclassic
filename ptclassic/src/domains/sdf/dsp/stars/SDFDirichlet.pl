defstar {
	name { Dirichlet }
	domain { SDF }
	desc {
This star computes the normalized Dirichlet kernel (a.k.a. the aliased
sinc function):
.EQ
d sub N (x) ~=~ { sin ( { N x } over 2 ) } over { N sin( x over 2 ) }
.EN
The value of the normalized Dirichlet kernel at $x = 0$ is always 1, and the
normalized Dirichlet kernel oscillates between $-1$ and $+1$.
The normalized Dirichlet kernel is periodic in $x$ with a period of either
$2 pi$ when $N$ odd or $4 pi$ when $N$ even.
	}
	version {$Id$}
	author { Brian Evans }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	seealso { Sinc }
	input {
		name{ input }
		type{ float }
	}
	output {
		name{ output }
		type{ float }
	}
	defstate {
		name { N }
		type { int }
		default { 10 }
		desc { Length of the Dirichlet kernel. }
	}
	ccinclude { <math.h> }
	go {
		const double DELTA 1.0e-9;
		double x = input%0;
		double sinInDenom = sin(x/2);
		double length = double(N);
		double dirichletValue;

		if ( ( -DELTA < sinInDenom ) && ( sinInDenom < DELTA ) )
		  // Use L'Hopital's Rule when sin(x/2) is approx. 0
		  dirichletValue = cos(length * x / 2) / cos(x / 2);
		else
		  // Otherwise, compute it using the definition
		  dirichletValue = sin(length * x / 2) / (length * sinInDenom);

		output%0 << dirichletValue;
	}
}

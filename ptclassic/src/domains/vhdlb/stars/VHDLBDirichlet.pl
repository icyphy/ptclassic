defstar {
	name { Dirichlet }
	domain {VHDLB}
        desc {
This star computes the normalized Dirichlet kernel (also called the aliased
sinc function):

d(x) = sin(N x / 2) / (N sin(x/2))

The value of the normalized Dirichlet kernel at x = 0 is always 1, and the
normalized Dirichlet kernel oscillates between -1 and +1.
The normalized Dirichlet kernel is periodic in x with a period of either
2*pi when N is odd or 4*pi when N is even.
        }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
The Dirichlet kernel is the discrete-time Fourier transform (DTFT) of a
sampled pulse function.  The "N" parameter is the length of the pulse.

In the calculation of the Dirichlet kernel, if the input X is very close to
zero (within +/- delta around zero), l'Hopital's rule is used to calculate
the value.  Otherwise the formula  sin( N * x/2 )/N*sin( x/2 )  is used to
calculate the value.
        }
	input {
		name { input }
		type { float }
		desc { input x to the Dirichlet kernel }
	}
	output {
		name { output }
		type { float }
	}
	state {
		name { N }
		type { int }
		default { 10 }
		desc { Length of the Dirichlet kernel }
	}
	go {
	}
}

defstar {
	name {ConstCx}
	domain {SDF}
	version {$Id$}
	desc {
Outputs a constant complex signal with real part "real"
(default 0.0) and imaginary part "imag" (default 0.0).
	}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
This star produces a complex DC output (default zero)
Even though we have ComplexState, the real and imaginary parts
are separate states.  (Should this change?)
	}
	ccinclude { "ComplexSubset.h" }
	output {
		name {output}
		type {complex}
	}
	defstate {
		name {real}
		type{float}
		default {"0.0"}
		desc {Real part of DC value.}
	}
	defstate {
		name {imag}
		type{float}
		default {"0.0"}
		desc {Imaginary part of DC value.}
	}
	go {
		output%0 << Complex(real,imag);
	}
}


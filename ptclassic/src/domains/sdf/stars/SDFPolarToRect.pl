defstar {
	name { PolarRect }
	domain { SDF }
	desc { Convert magnitude and phase to rectangular form. }
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name { magnitude }
		type { float }
	}
	input {
		name { phase }
		type { float }
	}
	output {
		name { x }
		type { float }
	}
	output {
		name { y }
		type { float }
	}
	ccinclude { <math.h> }
	go {
		double m = magnitude%0;
		double p = phase%0;
		x%0 << m * cos(p);
		y%0 << m * sin(p);
	}
}

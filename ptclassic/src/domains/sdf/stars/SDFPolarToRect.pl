defstar {
	name { PolarToRect }
	domain { SDF }
	desc { Convert magnitude and phase to rectangular form. }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	explanation {
Two floating-point inputs are converted into two floating-point outputs.
.Id "polar to rectangular conversion"
.Id "format conversion, polar to rectangular"
.Ir "magnitude"
.Ir "phase"
.Ir "Cartesian coordinates"
	}
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

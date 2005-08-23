defstar {
	name { PolarToRect }
	domain { SDF }
	desc { Convert magnitude and phase to rectangular form. }
	version { @(#)SDFPolarToRect.pl	1.12	10/01/96 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
Two floating-point inputs are converted into two floating-point outputs.
<a name="polar to rectangular conversion"></a>
<a name="format conversion, polar to rectangular"></a>
<a name="magnitude"></a>
<a name="phase"></a>
<a name="Cartesian coordinates"></a>
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

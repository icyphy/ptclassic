defstar {
	name { RectToPolar }
	domain { SDF }
	desc {
Converts the complex number x + j y, for inputs "x" and "y",
into magnitude and phase form.
The phase output is in the range -PI to PI.
	}
	version {@(#)SDFRectToPolar.pl	1.13 10/01/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<a name="rectangular to polar format conversion"></a>
<a name="format conversion, rectangular to polar"></a>
<a name="magnitude"></a>
<a name="phase"></a>
<a name="Cartesian coordinates"></a>
<a name="polar coordinates"></a>
	}
	input {
		name { x }
		type { float }
	}
	input {
		name { y }
		type { float }
	}
	output {
		name { magnitude }
		type { float }
	}
	output {
		name { phase }
		type { float }
	}
	go {
		double xpart = x%0;
		double ypart = y%0;
		double m = sqrt(xpart*xpart + ypart*ypart);
		magnitude%0 << m;
		if (m == 0)
		   phase%0 << 0.0;
		else	
		   phase%0 << atan2(ypart,xpart);
	}
}

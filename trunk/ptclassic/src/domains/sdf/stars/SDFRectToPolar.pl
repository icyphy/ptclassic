defstar {
	name { RectToPolar }
	domain { SDF }
	desc {
Converts the complex number x + j y, for inputs "x" and "y",
into magnitude and phase form.
The phase output is in the range -PI to PI.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Id "rectangular to polar format conversion"
.Id "format conversion, rectangular to polar"
.Ir "magnitude"
.Ir "phase"
.Ir "Cartesian coordinates"
.Ir "polar coordinates"
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

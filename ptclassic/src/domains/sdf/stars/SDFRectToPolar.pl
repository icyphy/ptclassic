defstar {
	name { RectPolar }
	domain { SDF }
	desc {
Convert two numbers to magnitude and phase.
The phase output is in the range -PI to PI.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
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

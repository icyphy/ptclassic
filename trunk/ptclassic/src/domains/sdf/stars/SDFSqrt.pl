defstar {
	name {Sqrt}
	domain {SDF}
	desc { This star computes the square root of its input.  }
	version {$Revision$ $Date$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	ccinclude { <math.h> }
	go {
		output%0 << sqrt (input%0);
	}
}

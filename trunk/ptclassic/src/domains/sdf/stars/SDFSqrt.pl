defstar {
	name {Sqrt}
	domain {SDF}
	desc { This star computes the square root of its input.  }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
.Id "square root"
	}
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
		output%0 << sqrt (double(input%0));
	}
}

defstar {
	name {Cos}
	domain {SDF}
	desc { This star computes the cosine of its input, in radians.  }
	version {$Id$}
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
	go {
		output%0 << cos (double(input%0));
	}
}

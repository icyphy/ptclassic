defstar {
	name {Sgn}
	domain {SDF}
	desc {
This star computes the signum of its input.
The output is +- 1.
	}
	version {$Id$}
	author { E. A. Lee }
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
		output%0 << copysign(1.0,(input%0));
	}
}

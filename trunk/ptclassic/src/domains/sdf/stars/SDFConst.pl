defstar {
	name {Const}
	domain {SDF}
	desc { Output a constant signal with value level (default 0.0).  }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	output {
		name {output}
		type {float}
	}
	defstate {
		name {level}
		type{float}
		default {"0.0"}
		desc {The constant value.}
	}
	go {
		output%0 << double(level);
	}
}


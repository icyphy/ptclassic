defstar {
	name {Const}
	domain {SDF}
	desc {
Output a constant signal with value given by the "level" parameter (default 0.0).
	}
	version {@(#)SDFConst.pl	1.8 3/2/95}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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


defstar {
	name {ConstInt}
	domain {SDF}
	desc {
Output a constant signal with value given by the "level"
parameter (default 0).
	}
	version { $Id$ }
	author { Brian L. Evans }
	acknowledge { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	output {
		name {output}
		type {int}
	}
	defstate {
		name {level}
		type {int}
		default {"0"}
		desc {The constant value.}
	}
	go {
		output%0 << int(level);
	}
}

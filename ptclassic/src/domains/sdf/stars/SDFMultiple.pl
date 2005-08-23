defstar {
	name { Multiple }
	domain { SDF } 
	desc { Output a 1 if top input is a multiple of bottom input }
	version {@(#)SDFMultiple.pl	1.4	3/2/95}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { top }
		type { int }
		desc { Is this a multiple of the other input? }
	}
	input {
		name { bottom }
		type { int }
		desc { Reference input (must be positive) }
	}
	output {
		name { mult }
		type { int }
		desc { Equals 1 if top is a multiple of bottom }
	}
	go {
		int t = top%0;
		int b = bottom%0;
		if (b <= 0) {
			Error::abortRun(*this,
			   "Reference input (bottom) is not positive");
			return;
		}
		mult%0 << (((t%b) == 0) ? 1 : 0);
	}
}

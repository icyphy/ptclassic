defstar {
	name { DC }
	domain {VHDLB}
	desc {
Output a constant signal with value level (default 0.0).
	}
        version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDLB main library }
	output {
		name { output }
		type { int }
	}
	defstate {
		name { level }
		type { int }
		default { 0.0 }
		desc { The constant value }
	}
	defstate {
		name { interval }
		type { int }
		default { 1 }
		desc { The interval between output transactions, in nsec }
	}
	go {
	}
}

defstar {
	name { xx74 }
	domain {VHDLB}
	desc {
D-Latch with clock, preset, clear inputs, Q and /Q outputs.
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
	input {
		name { D }
		type { int }
	}
	input {
		name { clock }
		type { int }
	}
	input {
		name { notPre }
		type { int }
	}
	input {
		name { notClr }
		type { int }
	}
	output {
		name { Q }
		type { int }
	}
	output {
		name { notQ }
		type { int }
	}
        defstate {
                name { delay }
                type { int }
                default { 1 }
                desc { The propagation delay, in nsec }
        }
	go {
	}
}

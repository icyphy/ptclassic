defstar {
	name { Register }
	domain {VHDLB}
	desc {
Remember and output the input value at the time of a positive clock edge.
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
		name { input }
		type { int }
	}
	input {
		name { clock }
		type { int }
	}
	output {
		name { output }
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

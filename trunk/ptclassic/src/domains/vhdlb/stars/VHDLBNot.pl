defstar {
	name { Not }
	domain {VHDLB}
	desc {
Output the logical inverse of the input with the given delay.
	}
        version { $Id$ }
	author { M. C. Williamson }
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

defstar {
	name { Through }
	domain {VHDLB}
	desc {
Output the input value.
	}
        version { @(#)VHDLBThrough.pl	1.5 3/2/95 }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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

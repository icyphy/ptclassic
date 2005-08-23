defstar {
	name { Xnor }
	domain {VHDLB}
	desc {
Output the logical xnor of the two inputs with the given delay.
	}
        version { @(#)VHDLBXnor.pl	1.3 3/2/95 }
	author { M. C. Williamson }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDLB main library }
	input {
		name { input1 }
		type { int }
	}
	input {
		name { input2 }
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

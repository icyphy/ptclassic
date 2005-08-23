defstar {
	name { Ramp }
	domain {VHDLB}
	desc {
Output a signal which increases by the stepsize each simulation step.
	}
        version { @(#)VHDLBRamp.pl	1.4 3/2/95 }
	author { M. C. Williamson }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
		name { init }
		type { int }
		default { 0 }
		desc { The initial value. }
	}
	defstate {
		name { step }
		type { int }
		default { 1 }
		desc { The constant stepsize. }
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

defstar {
	name { Ramp }
	domain {VHDLB}
	desc {
Output a signal which increases by the stepsize each simulation step.
	}
        version { $Id$ }
	author { M. C. Williamson }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
	go {
	}
}

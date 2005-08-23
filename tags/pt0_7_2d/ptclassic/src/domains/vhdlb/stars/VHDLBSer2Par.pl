defstar {
	name { Ser2Par }
	domain {VHDLB}
	desc {
Shift in eight bits on the serial input and output them on the parallel output.
Latch out a new output byte every eight clock cycles.
Shift the bits in (starting with the lowest) one per clock cycle.
	}
        version { @(#)VHDLBSer2Par.pl	1.5 3/2/95 }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDLB main library }
	input {
		name { serIn }
		type { int }
	}
	input {
		name { clock }
		type { int }
	}
	output {
		name { bus1 }
		type { int }
	}
	output {
		name { bus2 }
		type { int }
	}
	output {
		name { bus3 }
		type { int }
	}
	output {
		name { bus4 }
		type { int }
	}
	output {
		name { bus5 }
		type { int }
	}
	output {
		name { bus6 }
		type { int }
	}
	output {
		name { bus7 }
		type { int }
	}
	output {
		name { bus8 }
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

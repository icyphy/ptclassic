defstar {
	name { Par2Ser }
	domain {VHDLB}
	desc {
Output the eight bits of input in parallel on the serial output.
Latch in a new input byte every eight clock cycles.
Shift the bits out (starting with the lowest) one per clock cycle.
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
		name { bus1 }
		type { int }
	}
	input {
		name { bus2 }
		type { int }
	}
	input {
		name { bus3 }
		type { int }
	}
	input {
		name { bus4 }
		type { int }
	}
	input {
		name { bus5 }
		type { int }
	}
	input {
		name { bus6 }
		type { int }
	}
	input {
		name { bus7 }
		type { int }
	}
	input {
		name { bus8 }
		type { int }
	}
	input {
		name { clock }
		type { int }
	}
	output {
		name { serOut }
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

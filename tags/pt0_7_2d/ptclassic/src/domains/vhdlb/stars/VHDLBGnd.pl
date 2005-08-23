defstar {
	name { Gnd }
	domain {VHDLB}
        desc { Outputs a logical low every "interval" nanoseconds. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Outputs a zero value every "interval" nanoseconds.  Mimics a logical low
(ground) in a logic circuit.
        }
	state {
		name { interval }
		type { int }
		default { 1 }
		desc { output transaction interval in nanoseconds }
	}
	output {
		name { output }
		type { int }
	}
	go {
	}
}

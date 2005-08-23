defstar {
	name { FReg }
	domain {VHDLB}
        desc {
A simple rising-edge triggered register that saves one real number.
        }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
This star latches and stores a real number at every rising edge of
the synchronization clock.
        }
	state {
		name { delay }
		type { int }
		default { 1 }
		desc { internal delay in nanoseconds }
	}
	input {
		name { clock }
		type { int }
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	go {
	}
}

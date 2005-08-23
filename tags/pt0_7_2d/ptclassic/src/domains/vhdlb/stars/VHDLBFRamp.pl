defstar {
	name { FRamp }
	domain {VHDLB}
        desc { Outputs a ramp function. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
The real-valued output of this star starts at zero and increments by
"step" at every rising edge of the synchronization clock.
        }
	state {
		name { step }
		type { float }
		default { 1.0 }
	}
	input {
		name { clock }
		type { int }
	}
	output {
		name { output }
		type { float }
	}
	go {
	}
}

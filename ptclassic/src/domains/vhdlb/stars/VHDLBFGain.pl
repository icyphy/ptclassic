defstar {
	name { FGain }
	domain {VHDLB}
        desc { Multiplies a real input by a real-valued gain. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
        explanation {
A real-valued input number is multiplied by a user-specified real-valued gain.
        }
	state {
		name { gain }
		type { float }
		default { 1.0 }
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

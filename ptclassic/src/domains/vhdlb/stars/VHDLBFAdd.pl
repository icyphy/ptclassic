defstar {
	name { FAdd }
	domain {VHDLB}
	desc { Adds two real numbers }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Two real-valued input numbers are added together and sent to the output.
        }
	input {
		name { inputa }
		type { float }
	}
	input {
		name { inputb }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	go {
	}
}

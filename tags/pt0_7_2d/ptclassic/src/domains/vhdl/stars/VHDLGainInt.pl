defstar {
	name { GainInt }
	domain { VHDL }
	desc {
	}
	version { @(#)VHDLGainInt.pl	1.2 03/07/96 }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { gain }
		type { int }
		default { 1.0 }
	}
	codeblock (std) {
$ref(output) $assign(output) $ref(gain) * $ref(input);
	}
	go {
	  addCode(std);
	}
}

defstar {
	name { Ramp }
	domain { VHDL }
	desc {
Generates a ramp signal, starting at "value" (default 0)
with step size "step" (default 1).
	}
	version { $Id$ }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	output {
		name { output }
		type { int }
	}
	defstate {
		name { step }
		type { int }
		default { 1 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { int }
		default { 0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE | A_NONCONSTANT }
	}
	codeblock (std) {
$ref(output) $assign(output) $ref(value);
$ref(value) $assign(value) $ref(value) + $val(step);
	}
	go {
	  addCode(std);
	}
	exectime {
		return 2;
	}
}

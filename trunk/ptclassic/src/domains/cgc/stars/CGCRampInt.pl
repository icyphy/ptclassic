defstar {
	name { RampInt }
	domain { CGC }
	desc {
Generates a ramp signal, starting at "value" (default 0)
with step size "step" (default 1).
	}
	version { @(#)CGCRampInt.pl	1.1	1/22/96 }
	author { Siamak Modjtahedi }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go { addCode(std); }
	codeblock (std) {
	$ref(output) = $ref(value);
	$ref(value) += $val(step);
	}
	exectime {
		return 2;
	}
}

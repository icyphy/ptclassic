defstar {
	name { DownCounter }
	domain { CGC }
	desc { 
A down counter from the input value to zero. 
Code is generated for each step operation instead of generating all outputs
at once.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
This star generates (int-1), (int-2), ... 1,0 samples for (int) input.
Input should be a positive integer (can not be zero).
	}
	input {
		name { input }
		type { int }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	state {
		name { status }
		type { int }
		default { "0" }
		desc { monitoring the control value }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	codeblock (block) {
	if ($ref(status) == 0) {
		if ($ref(input) > 0) {
			$ref(output) = $ref(input) - 1;
		} 
	} else {
		$ref(output) = $ref(output) - 1;
	}
	$ref(status) = $ref(output);
	}
	go {
		addCode(block);
	}
}


defstar {
	name { SumOfN }
	domain { CGC }
	desc {
Outputs the sum of N input tokens,
where N is the control input.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =input }
	}
	state {
		name { status }
		type { int }
		default { "0" }
		desc { "monitor how many tokens added" }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	codeblock (block) {
	if ($ref(status) == 0) {
		$ref(output) = 0;
		$ref(status) = $ref(control);
	}
	$ref(output) += $ref(input);
	$ref(status)--;
	}
	go {
		addCode(block);
	}
}



defstar {
	name { LastOfN }
	domain { CGC }
	desc {
Outputs the last token of N input tokens,
where N is the control input.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { ANYTYPE }
		num { 0 }
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
		desc { monitoring the control value }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	method {
		name { readTypeName }
		access { public }
		type { "const char *" }
		code { return "DownSample"; }
	}

	codeblock (block) {
	if ($ref(status) == 0)
		$ref(status) = $ref(control);
	if ($ref(status) == 1)
		$ref(output) = $ref(input);
	$ref(status)--;
	}
	go {
		addCode(block);
	}
}



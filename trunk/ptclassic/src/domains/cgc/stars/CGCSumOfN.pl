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
Copyright (c) 1990-%Q% The Regents of the University of California.
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
		desc { "monitor how many tokens added" }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	method {
		name { readTypeName }
		access { public }
		type { "const char *" }
		code { return "DownSample"; }
	}

	codeblock (nonComplexBlock) {
	if ($ref(status) == 0) {
		$ref(output) = 0;
		$ref(status) = $ref(control);
	}
	$ref(output) += $ref(input);
	$ref(status)--;
	}
	codeblock (complexBlock) {
	if ($ref(status) == 0) {
		$ref(output).real = 0;
		$ref(output).imag = 0;
		$ref(status) = $ref(control);
	}
	$ref(output) += $ref(input);
	$ref(status)--;
	}
	go {
		if (strcmp(input.resolvedType(), "COMPLEX") == 0) 
		  addCode(complexBlock);
		else
		  addCode(nonComplexBlock);
	}
}



defstar {
	name { FloatToCx }
	domain { CGC }
	desc { type conversion from float/int to complex }
	version { $Id$ }
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { complex }
	}
	state {
		name { numSample }
		type { int }
		default { "1" }
		desc { number of samples to be converted }
	}
	constructor {
		noInternalState();
	}
	setup {
		if (int(numSample) > 1) {
			input.setSDFParams(int(numSample));
			output.setSDFParams(int(numSample));
		}
	}
	initCode {
		numSample = output.numXfer();
	}
	codeblock (body) {
	int i;
	for (i = 0; i < $val(numSample); i++) {
		$ref(output, i).real = $ref(input, i);
		$ref(output, i).imag = 0;
	}
	}
	go {
		addCode(body);
	}
	exectime {
		return 2 * int(numSample);
	}
}


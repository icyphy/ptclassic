defstar {
	name { FloatToInt }
	domain { CGC }
	desc { type conversion from float to int }
	version { $Id$ }
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { int }
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
		$ref(output, i) = (int) $ref(input, i);
	}
	}
	go {
		addCode(body);
	}
	exectime {
		return int(numSample);
	}
}


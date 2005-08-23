defstar {
	name { FloatToInt }
	domain { CGC }
	desc { type conversion from float to int }
	version { @(#)CGCFloatToInt.pl	1.4	03/27/97 }
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
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
		addInclude("<math.h>");
		numSample = output.numXfer();
	}
	codeblock (body) {
	int i = 0;
	for (; i < $val(numSample); i++) {
		$ref(output, i) = (int) floor($ref(input, i) + 0.5);
	}
	}
	go {
		addCode(body);
	}
	exectime {
		return int(numSample);
	}
}


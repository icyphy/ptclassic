defstar {
	name { Gain }
	domain { CGC }
	desc { Amplifier: output is input times "gain" (default 1.0). }
	version { $Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { gain }
		type { float }
		default { "1.0" }
		desc { Gain of the star. }
	}
	constructor {
		noInternalState();
	}
	go {
		addCode("$ref(output) = $val(gain) * $ref(input);\n");
	}
}


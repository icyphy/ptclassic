defstar {
	name { RectToCx }
	domain { CGC }
	desc {Convert real and imaginary parts to a complex output.}
	version { $Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	input {
		name { real }
		type { float }
	}
	input {
		name { imag }
		type { float }
	}
	output {
		name { output }
		type { complex }
	}
	constructor {
		noInternalState();
	}
	codeblock(body) {
	$ref(output).real = $ref(real);
	$ref(output).imag = $ref(imag);
	}
	
	go {
		addCode(body);
	}
}

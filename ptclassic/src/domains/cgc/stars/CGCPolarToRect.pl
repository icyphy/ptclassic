defstar {
	name { PolarToRect }
	domain { CGC }
	desc { Convert magnitude and phase to rectangular form. }
	version { $Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	input {
		name { magnitude }
		type { float }
	}
	input {
		name { phase }
		type { float }
	}
	output {
		name { x }
		type { float }
	}
	output {
		name { y }
		type { float }
	}
	constructor {
		noInternalState();
	}
	setup {
		addInclude("<math.h>");
	}
	codeblock(body) {
	double m,p;
	m = $ref(magnitude);
	p = $ref(phase);
	$ref(x) = m * cos(p);
	$ref(y) = m * sin(p);
	}
	go {
		addCode(body);
	}
}

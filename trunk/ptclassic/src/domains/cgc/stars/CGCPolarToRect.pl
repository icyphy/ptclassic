defstar {
	name { PolarToRect }
	domain { CGC }
	desc { Convert magnitude and phase to rectangular form. }
	version { @(#)CGCPolarToRect.pl	1.7	01/01/96 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	initCode {
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
	exectime {
		return 50;
	}
}

defstar {
	name { RectToPolar }
	domain { CGC }
	desc {
Convert two numbers to magnitude and phase.
The phase output is in the range -PI to PI.
	}
	version { $Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	input {
		name { x }
		type { float }
	}
	input {
		name { y }
		type { float }
	}
	output {
		name { magnitude }
		type { float }
	}
	output {
		name { phase }
		type { float }
	}
	constructor {
		noInternalState();
	}
	setup {
		addInclude("<math.h>");
	}
	codeblock(body) {
	double xpart, ypart, m;
	xpart = $ref(x);
	ypart = $ref(y);
	m = sqrt(xpart*xpart + ypart*ypart);
	$ref(magnitude) = m;
	if (m == 0) $ref(phase) = 0;
	else $ref(phase) = atan2(ypart,xpart);
	}
	
	go {
		  addCode(body); 
	}
}

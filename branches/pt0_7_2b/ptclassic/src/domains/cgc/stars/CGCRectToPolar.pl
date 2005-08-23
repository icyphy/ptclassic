defstar {
	name { RectToPolar }
	domain { CGC }
	desc {
Convert two numbers to magnitude and phase.
The phase output is in the range -PI to PI.
	}
	version { @(#)CGCRectToPolar.pl	1.7	01/01/96 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
	initCode {
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
	exectime {
		return 200;	/* based on CG96 stars */
	}
}

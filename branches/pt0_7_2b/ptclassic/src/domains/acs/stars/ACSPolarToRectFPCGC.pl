defcore {
    name { PolarToRect }
    domain { ACS }
    coreCategory { FPCGC }
    corona { PolarToRect } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSPolarToRectFPCGC.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

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

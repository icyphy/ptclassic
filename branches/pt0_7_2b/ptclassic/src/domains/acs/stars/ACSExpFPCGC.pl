defcore {
    name { Exp }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Exp } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSExpFPCGC.pl	1.4 09/08/99}
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
	go {
		addCode(exp);
	}
   codeblock(exp) {
	$ref(output) = exp($ref(input));
   }
	exectime {
		return 33;	/* use the same value as log */
	}
}

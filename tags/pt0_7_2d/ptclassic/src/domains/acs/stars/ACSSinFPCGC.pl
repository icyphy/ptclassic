defcore {
    name { Sin }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Sin } 
    desc { Output the product of the inputs, as a floating-point value. }
    version { @(#)ACSSinFPCGC.pl	1.4 09/08/99}
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
		addCode(singen);
	}
   codeblock(singen) {
	$ref(output) = cos($ref(input));
   }
	exectime {
		return 23;	/* value taken from CG96Sin */
	}
}

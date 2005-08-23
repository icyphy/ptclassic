defcore {
    name { Sqrt }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Sqrt } 
    desc { This star computes the square root of its input. }
    version { @(#)ACSSqrtFPCGC.pl	1.2 09/08/99 }
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
		addCode(sqrtgen);
	}
   codeblock(sqrtgen) {
	$ref(output) = sqrt($ref(input));
   }
	exectime {
		return 165;	/* value taken from CG96Sin */
	}
}

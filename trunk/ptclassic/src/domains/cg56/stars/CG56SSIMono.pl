defstar {
    name { SSIMono }
    domain { CG56 }
    desc { A generic input/output star the DSP56001 SSI port. }
    version { $Id$ }
    author { Jose Luis Pino }
    derivedFrom { SSI }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 io library }
    explanation {
    }
    seealso { ProPortAD, ProPortDA, MagnavoxIn, MagnavoxOut }
    hinclude { <stream.h> }
    constructor {
	removePort(input2);
	removePort(output2);
	doRecv1 = TRUE;
	doRecv2 = FALSE;
	doXmit1 = TRUE;
	doXmit2 = FALSE;
    }
}    

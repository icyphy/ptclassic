defstar {
    name { SSIMono }
    domain { CG56 }
    desc { A generic input/output star the DSP56001 SSI port. }
    version { @(#)CG56SSIMono.pl	1.6 03/29/97 }
    author { Jose Luis Pino }
    derivedFrom { SSI }
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
    htmldoc {
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

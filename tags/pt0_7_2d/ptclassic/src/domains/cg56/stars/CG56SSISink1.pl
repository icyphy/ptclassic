defstar {
    name { SSISink1 }
    domain { CG56 }
    desc { A generic input/output star the DSP56001 SSI port. }
    version { @(#)CG56SSISink1.pl	1.5 03/29/97 }
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
	removePort(output1);
	removePort(output2);
    }
    begin {
	doRecv1 = doRecv2 = FALSE;
	doXmit1 = TRUE;
	doXmit2 = FALSE;
    }
}    

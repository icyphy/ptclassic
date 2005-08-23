defstar {
    name { IntToFix }
    domain { C50 }
    desc { Convert an integer input to a fixed-point output. }
    version { @(#)C50IntToFix.pl	1.3  07/26/96  }
    author { Luis Gutierrez }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 main library }
    input {
	name {input}
	type {INT}
    }
    output {
	name {output}
	type {FIX}
	attributes { P_NOINIT }
    }
    constructor {
	noInternalState();
    }
    setup {
	forkInit(input,output);
    }

    exectime { return 0; }
}

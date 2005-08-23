defstar {
    name { FixToInt }
    domain { C50 }
    desc { Convert a fixed-point input to an integer output. }
    version {@(#)C50FixToInt.pl	1.5	05/26/98}
    author { Luis Gutierrez, G. Arslan }
    copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 main library }
    input {
	name {input}
	type {FIX}
    }
    output {
	name {output}
	type {INT}
	
    }
    constructor {
	noInternalState();
    }
    setup{
	forkInit(input,output);
    }

    exectime { return 0; }
}

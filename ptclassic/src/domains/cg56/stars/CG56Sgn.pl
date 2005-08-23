defstar {
	name {Sgn}
	domain {CG56}
	desc {
This star outputs the signum of its input.  The output is either -1.0
or +0.99999999, since 1.0 is not a valid fixed point number on the DSP56001.
	}
	version {@(#)CG56Sgn.pl	1.11	01 Oct 1996}
	acknowledge { Gabriel version by E. A. Lee }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
       	location { CG56 main library }
	htmldoc {
<a name="signum"></a>
	}
	input {
		name{input}
		type{FIX}
	}
	output {
		name{output}
		type{FIX}
	}
	constructor {
		noInternalState();
	}
	codeblock (sgnblock) {
        move    $ref(input),a
        move    #.99999999,b
        move    #-1.0,x0
  	tst     a            
        tmi     x0,b
        move    b,$ref(output)
	}	
	go {
		addCode(sgnblock);
 
	}
	execTime {
		return 6;
	}
}

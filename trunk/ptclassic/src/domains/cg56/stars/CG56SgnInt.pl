defstar {
	name {SgnInt}
	domain {CG56}
	desc {
   	Signum.
	}
	version {$Id$ }
	acknowledge { Gabriel version by E. A. Lee }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
       	location { CG56 nonlinear functions library }
	explanation { 
This star outputs the signum of its input.
.Id "signum"
The output is either 0 or 1
	}
	execTime {
		return 6;
	}
	input {
		name{input}
		type{FIX}
	}
	output {
		name{output}
		type{INT}
	}
	codeblock (sgnblock) {
        move    $ref(input),a
        move    #1,b1
        move    #0,x0
  	tst     a            
        tmi     x0,b
        move    b1,$ref(output)
	}	

	go {
		addCode(sgnblock);
 
	}
}

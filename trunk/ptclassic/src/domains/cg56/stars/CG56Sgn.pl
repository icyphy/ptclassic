defstar {
	name {Sgn}
	domain {CG56}
	desc {
   	Signum.
	}
	version {$Id$}
	acknowledge { Gabriel version by E. A. Lee }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
       	location { CG56 Demo Library }
	explanation { 
This star outputs the signum of its input.
.Id "signum"
The output is either -1.0 or +0.99999999, since 1.0 is not
a valid fixed point number on the DSP56001.
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
		type{FIX}
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
}

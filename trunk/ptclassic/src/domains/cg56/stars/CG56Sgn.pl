defstar {
	name {Sgn}
	domain {CG56}
	desc {
   	Signum.
	}
	version {$Id$}
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
       	location { CG56 Demo Library }
	explanation { 
This star computes the signum of its input.
The output is +- 1 because 1 is not allowed 
for fix input, .99999999 is used instead.
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
		gencode(sgnblock);
 
	}
}

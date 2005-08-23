defstar {
	name { Ramp }
	domain { CG56 }
	desc {Ramp generator}
	version {@(#)CG56Ramp.pl	1.17 03/29/97}
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
Generates a ramp signal, starting at <i>value</i> (default 0.0)
with step size <i>step</i> (default 0.001).
	}
	output {
		name { output }
		type { fix }
	}
	state {
		name { step }
		type { fix }
		default { 0.001 }
		desc { Increment from one sample to the next. }
	}
	state {
		name { value }
		type { fix }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
        state {
                name {sum}
                type {FIX}
                default {0}
                desc { An internal state.}
                attributes { A_NONSETTABLE|A_YMEM|A_NONCONSTANT }
	}

        codeblock (main) {
	move	$ref(sum),a
	move	#$val(step),x0
        move    a1,$ref(output)
	add	x0,a
        move    a1,$ref(sum)
	}
	
	setup {
		sum=value;
	}
	go {
		addCode(main);
	}
        execTime {
		return 5;
	}
}

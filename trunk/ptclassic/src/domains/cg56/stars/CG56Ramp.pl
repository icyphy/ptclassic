defstar {
	name { Ramp }
	domain { CG56 }
	desc {
Generates a ramp signal, starting at "value" (default 0.0)
with step size "step" (default 0.001).
	}
	version {$Id$}
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 main library }
	explanation {
.Id "ramp generator"
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
	
	start {
		sum=value;
	}
	go {
		addCode(main);
	}
        execTime {
		return 5;
	}
}

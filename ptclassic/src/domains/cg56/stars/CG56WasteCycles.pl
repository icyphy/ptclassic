defstar {
	name { WasteCycles }
	domain { CG56 }
	desc {
A star to stall the flow of data for parameter cycles to waste cycles.
        }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
A star to stall the flow of data for parameter cycles to waste cycles.
        }
        input {
                name { input }
                type { fix }
        }	    
        output {
		name { output }
		type { fix }
	}
	state {
		name { cyclesToWaste }
		type { int }
		desc { numbers of cycles to waste }
		default { 10 }
	}
	state {
		name { X }
		type { int }
		desc { internal }
		default { 0 }
	}
	codeblock(std) {
	move	$ref(input),x1
        rep     #$val(X)
        nop
        move    x1,$ref(output)
	} 
        go {
                X=cyclesToWaste-4;
                addCode(std);
	}
	execTime { 
		return int(cyclesToWaste);
	}
}

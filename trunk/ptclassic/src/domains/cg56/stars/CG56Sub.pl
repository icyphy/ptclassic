defstar {
	name { Sub }
	domain { CG56 }
	desc { any input subtractor }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.PP
Output the \fIpos\fR minus all \fIneg\fR inputs.
        }    
	input {
		name { pos }
		type { fix }
 	}
	inmulti {
		name { neg }
		type { fix }
 	}
	output {
		name { output }
		type { fix }
	}
        state  {
                name { inputNum }
                type { int }
                default { 0 }
                desc { input#() }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
        codeblock(main) {
        move    $ref(pos),a
        }
        codeblock(loop) {
        move    $ref(neg#inputNum),x0
        sub     x0,a
        }
        codeblock(done) {
        move    a,$ref(output)
        }

	go {
		gencode(main);
		for (int i = 1; i <= neg.numberPorts(); i++) {
			inputNum = i;
			gencode(loop);
		}
		gencode(done);
	}
	execTime {
		return 2 * int  (neg.numberPorts()) + 2;
	}
}

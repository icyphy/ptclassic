defstar {
	name { Sub }
	domain { CG56 }
	desc { any input subtractor }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 arithmetic library }
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
		addCode(main);
		for (int i = 1; i <= neg.numberPorts(); i++) {
			inputNum = i;
			addCode(loop);
		}
		addCode(done);
	}
	execTime {
		return 2 * int  (neg.numberPorts()) + 2;
	}
}

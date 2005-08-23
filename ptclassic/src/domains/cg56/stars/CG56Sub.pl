defstar {
	name { Sub }
	domain { CG56 }
	desc { any input subtractor }
	version { @(#)CG56Sub.pl	1.12	06 Oct 1996 }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<p>
Output the <i>pos</i> minus all <i>neg</i> inputs.
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
	constructor {
		noInternalState();
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

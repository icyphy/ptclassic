defstar {
	name { Impulse }
	domain { CG56 }
	desc { Impulse generator }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 signal sources library }
        explanation {
The star produces at its output an impulse with height given by the parameter
\fIimpulseSize\fR.
.PP
A state variable is maintained to keep around the next output value.
At initialization, the state is set to \fIimpulseSize\fR.
During each subsequent invocation, the output value is taken from the state,
which is then set to zero.
        }
        seealso { Quasar, DC }
        output {
		name { output }
		type { fix }
	}
	state {
		name { impulseSize }
		type { fix }
		desc { impulse size }
		default { ONE }
	}
 	state {
		name { pulse }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
	}
 	setup {
	           pulse=impulseSize;
	      }

	codeblock(std) {
	clr	b	$ref(pulse),a
	move	a,$ref(output)
	move	b,$ref(pulse)
	} 

        go { addCode(std); }
	execTime { 
		return 3;
	}
 }

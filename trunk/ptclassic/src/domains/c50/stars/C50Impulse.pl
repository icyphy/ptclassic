defstar {
	name { Impulse }
	domain { C50 }
	desc { Impulse generator }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 signal sources library }
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
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
	}
 	setup {
	           pulse=impulseSize;
	      }

	codeblock(std) {
	zap					;clear P-Reg. and Accu
	mar	*,AR5
	lar	AR5,#$addr(pulse)		;Address pulse 		=>AR5
	bldd	*,#$addr(output)		;value pulse => output
	sacl	*				;value pulse = 0
	} 

        go { addCode(std); }
	execTime { 
		return 6;
	}
}

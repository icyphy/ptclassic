defstar {
	name { Impulse }
	domain { CG56 }
	desc { Impulse generator }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 signal sources library }
        explanation {
The star produces at its output an impulse train with height given by the 
parameter \fIimpulseSize\fR and period given by \fIperiod\fR which is 0 for
a single impulse.
.PP
A state variable is maintained to keep around the next output value.
At initialization, the state is set to \fIimpulseSize\fR.

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
		name { period }
		type { int }
		desc { period of the impulse train( 0 for one impulse) }
		default { 0 }
		attributes { A_CONSTANT|A_YMEM }
	} 
	state {
		name { counter }
		type { int }
		desc { internal counter }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM}
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
		   counter = 0;
		   if ( int(period) < 0 ){
			Error::abortRun( *this, 
				"Period must be non-negative.");
		   };
			
	      }

	codeblock(multiple) {
	move 	$ref(counter),a
	move	#1,r0
	clr	b	$ref(period),x0
	cmp 	x0,a	r0,x0
	tge	b,a
	tst	a	$ref(pulse),y0
	teq	y0,b	
	add	x0,a	b,$ref(output)
	move	a,$ref(counter)
	} 
	
	codeblock(single){
	clr	a	$ref(pulse),b
	move 	b,$ref(output)
	move	a,$ref(pulse)
	}

        go { 
		if (int(period) > 0) {
			addCode(multiple); 
		} else {
			addCode(single);
		}
	}

	execTime { 
		return 9;
	}
 }


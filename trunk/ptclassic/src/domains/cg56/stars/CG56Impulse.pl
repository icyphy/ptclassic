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
a single impulse.  The impulse train or pulse can be delayed by setting
the parameter \fIdelay\fR to a positive value.
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
		name { delay }
		type { int }
		desc { output will be delayed by this amount }
		default { 0 }
	}
	state {
		name { pulse }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
	}
	protected {
		int time;
	}
 	setup {
	           pulse=impulseSize;
		   counter = 0;
		   time = 0;
		   if ( int(period) < 0 ){
			Error::abortRun( *this, 
				"Period must be non-negative.");
		   };
		   if ( int(delay) < 0 ){
		   	Error::abortRun( *this,
		   		"Delay must be non-negative.");
		   };
		   if ( int(period) == 0 ){
		   	counter = - int(delay);
		   } else {
		   	counter = -(int(delay)%int(period));
		   };	
	      }

	codeblock(multiple) {
	move 	$ref(counter),a
	clr	b	$ref(period),x0
	cmp 	x0,a	#>(0.0000001192093),x1
	tge	b,a
	tst	a	$ref(pulse),y0
	teq	y0,b	
	add	x1,a	b,$ref(output)
	move	a,$ref(counter)
	} 
	
	codeblock(delayedSingle){        
	clr	b	$ref(period),x1
	move	b,x0	$ref(counter),a
	cmp 	x1,a	#>(0.0000001192093),y1
	tge	b,a
	tst	a	$ref(pulse),y0
	teq	y0,b
	move	b,$ref(output)
	move	x0,b
	tne	y0,b	
	add	y1,a	b,$ref(pulse)
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
			time = 8; 
		} else if ( int(delay) ){ 
			addCode(delayedSingle);
			time = 11;
		} else {
			addCode(single);
			time = 3;
		}
	}

	execTime { 
		return (time);
	}
 }






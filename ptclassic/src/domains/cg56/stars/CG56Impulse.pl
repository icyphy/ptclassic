defstar {
	name { Impulse }
	domain { CG56 }
	desc {
Generate a single impulse or an impulse train.  The impulse(s) have
amplitude "level" (default 1.0).  If "period" (default 0) is equal to 0,
then only a single impulse is generated; otherwise, it specifies the
period of the impulse train.  The impulse or impulse train is delayed
by the amount specified by "delay".
	}
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
The star produces at its output an impulse train with height given by the 
parameter <i>level</i> and period given by <i>period</i> which is 0 for
a single impulse.  The impulse train or pulse can be delayed by setting
the parameter <i>delay</i> to a positive value.
<p>
A state variable is maintained to keep around the next output value.
At initialization, the state is set to <i>level</i>.
	}
	seealso { Quasar, DC }
	output {
		name { output }
		type { fix }
	}
	state {
		name { level }
		type { fix }
		desc { Height of the impulse }
		default { ONE }
	}
	state {
		name { period }
		type { int }
		desc {
Non-negative period of the impulse train (0 means aperiodic)
		}
		default { 0 }
		attributes { A_CONSTANT|A_YMEM }
	} 
	state {
		name { count }
		type { int }
		desc { internal counter }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
	}
	state {
		name { delay }
		type { int }
		desc {
Non-negative delay on the output (0 means no delay)
		}
		default { 0 }
	}
	state {
		name { pulse }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
	}
 	setup {
		pulse = level;
		count = 0;
		if ( int(period) < 0 ) {
			Error::abortRun(*this, "Period must be non-negative.");
		}
		if ( int(delay) < 0 ) {
			Error::abortRun( *this, "Delay must be non-negative.");
		}
		if ( int(period) == 0 ) {
			count = - int(delay);
		}
		else {
		   	count = -(int(delay)%int(period));
		}
	}

	codeblock(multiple) {
	move 	$ref(count),a
	clr	b	$ref(period),x0
	cmp 	x0,a	#>(0.0000001192093),x1
	tge	b,a
	tst	a	$ref(pulse),y0
	teq	y0,b	
	add	x1,a	b,$ref(output)
	move	a,$ref(count)
	} 
	
	codeblock(delayedSingle) {
	clr	b	$ref(period),x1
	move	b,x0	$ref(count),a
	cmp 	x1,a	#>(0.0000001192093),y1
	tge	b,a
	tst	a	$ref(pulse),y0
	teq	y0,b
	move	b,$ref(output)
	move	x0,b
	tne	y0,b	
	add	y1,a	b,$ref(pulse)
	move	a,$ref(count)
	}	
		
	codeblock(single){
	clr	a	$ref(pulse),b
	move 	b,$ref(output)
	move	a,$ref(pulse)
	}

	go { 
		if ( int(period) > 0 ) {
			addCode(multiple);
		}
		else if ( int(delay) ) {
			addCode(delayedSingle);
		}
		else {
			addCode(single);
		}
	}

	execTime { 
		int time = 0;
		if ( int(period) > 0 ) time = 8;
		else if ( int(delay) ) time = 11;
		else time = 3;
		return (time);
	}
}

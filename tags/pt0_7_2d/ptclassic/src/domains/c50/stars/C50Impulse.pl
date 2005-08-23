defstar {
	name { Impulse }
	domain { C50 }
	desc { Impulse generator }
	version {@(#)C50Impulse.pl	1.8	05/26/98}
	author { Luis Gutierrez, A. Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
The star produces at its output an impulse with height given by the parameter
<i>level</i>.
<p>
A state variable is maintained to keep around the next output value.
At initialization, the state is set to <i>level</i>.
During each subsequent invocation, the output value is taken from the state,
which is then set to zero.
        }
        seealso { Quasar, Const }
        output {
		name { output }
		type { fix }
	}

	state {
		name { level }
		type { fix }
		desc { impulse size }
		default { ONE }
	}

	state {
		name { period }
		type { int }
		desc { 
Non-negative period of the impulse train (0 means aperiodic)
		}
		default { 0 }
	} 

	state {
		name { delay }
		type { int }
		desc {
Non-negative delay on the output (0 means no delay)
		}
		default { 0}
	} 

	state {
		name { count }
		type { int }
		desc { internal counter }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM|A_CONSEC }
	}

 	state {
		name { pulse }
		type { FIX }
		desc { internal }
		default { 0 }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
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
		if (int(period) == 1) output.setAttributes(P_NOINIT);
		if ( int(period) == 0 ) {
			count = 0;
		}
		else {
		   	count = int(period)-(int(delay)%int(period)) - 1;
		}
	}

	initcode{
	// period = 1 is equiv to a constant so just
		if (int(period) == 1) {
			StringList constant;
			constant.initialize();
			constant <<"lacl $addr(pulse)\n"
			         << "sacl $addr(output)\n";
			addCode(constant);
		}
	}

	codeblock(single) {
	zap					;clear P-Reg. and Accu
	mar	*,AR5
	lar	AR5,#$addr(pulse)		;Address pulse 		=>AR5
	bldd	*,#$addr(output)		;value pulse => output
	sacl	*				;value pulse = 0
	} 


	codeblock(multiple){
	lar	ar0,#$addr(count)
	mar	*,ar0
	lacc	#$val(period),0
	sacb	
	lacc	*+,0
	add	#1,0
	crlt	
	lar	ar1,#$addr(output)
	xc	1,NC
	lacc	*,16
	mar	*-,ar1
	sach	*,0,ar0
	sacl	*
	}

	codeblock(delayedSingle,""){
	lar	ar0,#$addr(count)
	mar	*,ar0
	lacc	#@(int(delay)+1),0
	sacb
	lacc	*,0
	add	#1,0
	crlt
	sacl	*+,0
	lacc	*,16
	xc	1,NC
	lacc	*,0
	sach	*,0,ar1
	lar	ar1,#$addr(output)
	sacl	*,0
	}
	
		
        go {
		if ( int(period) > 1 ) {
			addCode(multiple);
		}
		else if (int(period) == 1) {
			return;
		}
		else if ( int(delay) ) {
			addCode(delayedSingle());
		}
		else {
			addCode(single);
		}
	}

	execTime { 
		int time = 0;
		if ( int(period) > 1 ) time = 13;
		else if (int(period)==1) time = 0;
		else if ( int(delay) ) time = 14;
		else time = 5;
		return (time);
	}
}








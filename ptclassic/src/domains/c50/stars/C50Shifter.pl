defstar {
	name { Shifter }
	domain { C50 }
	desc { Hard shifter. }
	version { $Id$ }
	author { Luis Gutierrez, A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
.Id "bit shifter"
Scale by shifting left \fIleftShifts\fP bits.
Negative values of N implies right shifting.
Arithmetic shifts are used.
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {leftShifts}
		type {INT}
		default { 1 }
		desc { Number of left shifts. }
	}

	codeblock(shift,""){
	mar	*,ar1
	lar	ar1,#$addr(input)
	lar	ar2,#$addr(output)
	lacc	*,@(int(shifts)),ar2
	}
	
	codeblock(storeHigh){
	sach	*,0
	}
	
	codeblock(storeLow){
	sacl	*,0
	}	

	constructor {
		noInternalState();
	}
	
	protected{
		int shifts;
	}

	setup {
		if ((int(leftShifts) > 15) || (int(leftShifts) < -15)) {
			shifts = 0;
			return;
		}
		if (int(leftShifts) < 0 ) {
			shifts = 16 + int(leftShifts);
		} else {
			shifts = int(leftShifts);
		}
	}

	go {
		addCode(shift());
		if (int(leftShifts) >= 0) 
			addCode(storeLow);
		else
			addCode(storeHigh);
	}

	exectime {
		return 5;
	}
}

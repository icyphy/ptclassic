defstar {
	name { Shifter }
	domain { C50 }
	desc { Hard shifter. }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 arithmetic library }
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
	state {
		name {absShifts}
		type {INT}
		desc { Absolute value of number of bit shifts. }
		default { 0 }
		attributes { A_NONSETTABLE }
	}

	setup {
		int scnt = int(leftShifts);
		absShifts = (scnt >= 0) ? scnt : - scnt;
		absShifts = absShifts - 1;
	}

	codeblock(cbLoad) {
	mar 	*,AR6
	lar	AR6,#$addr(input)		;Address input		=> AR6
	lar	AR7,#$addr(output)		;Address output		=> AR7
        lacc    *,15,AR7			;Accu = input
	}

	codeblock(cbShifts) {
        rpt     #$val(absShifts)		;repeat number of shifts
	.if	#$val(leftShifts)>0		;
          sfl     				;left shifts
	.else					;
          sfr					;right shifts
	.endif					;
	}

	codeblock(cbSave) {
	sach	*,1				;output = Accu
	}

	go {
		addCode(cbLoad);
		if ( int(absShifts) != 0 ) addCode(cbShifts);
		addCode(cbSave);
	}

	exectime {
		return int(absShifts) + 5;
	}
}

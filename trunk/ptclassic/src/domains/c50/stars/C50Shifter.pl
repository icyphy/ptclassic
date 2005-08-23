defstar {
	name { Shifter }
	domain { C50 }
	desc { Hard shifter. }
	version { @(#)C50Shifter.pl	1.6	10/11/96 }
	author { Luis Gutierrez, A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
Scale by shifting left <i>leftShifts</i> <a name="bit shifter">bits</a>.
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
	    name { saturation }
	    type { int }
	    default { "YES" }
	    desc { If true, use saturation arithmetic }
	}
	codeblock(shift,""){
	clrc	sxm
	mar	*,ar1
	lar	ar1,#$addr(input)
	lar	ar2,#$addr(output)
	lacc	*,@(int(shifts)),ar2
	setc	sxm
	}
	
	codeblock(storeHigh){
	sach	*,0
	}
	
	codeblock(storeLow){
	sacl	*,0
	}	

	codeblock(saturateShift,""){
	setc	ovm	; set overflow mode
	splk	#@(int(leftShifts)-1),brcr
	mar	*,ar1
	lar	ar1,#$addr(input)
	lar	ar2,#$addr(output)
	lacc	*,16,ar2
	sacb
	rptb	$starSymbol(shft)
	addb	; shift left by 1
$starSymbol(shft)
	sacb
	sach	*,0
	clr	ovm
	}

	constructor {
		noInternalState();
	}
	
	protected{
		int shifts;
	}

	setup {
		if (int(leftShifts) < -15 ) {
//more than 15 right shifts is the same as outputing 0
			shifts = 0;
			return;
		}
		if ((int(leftShifts) > 15)){
//more than 15 left shifts and no sat. is equiv. to shifting 16 times
			shifts = 16;
			return;
		}
		if (int(leftShifts) < 0 ) {
			shifts = 16 + int(leftShifts);
		} else {
			shifts = int(leftShifts);
		}
	}

	go {
		if (int(saturation) && (int(leftShifts) >0)){
// only need to worry about saturation with left shifts > 0
			addCode(saturateShift());
			return;
		} else {
			addCode(shift());
		}
		if (int(leftShifts) >= 0) 
			addCode(storeLow);
		else
			addCode(storeHigh);
	}

	exectime {
		return 5;
	}
}

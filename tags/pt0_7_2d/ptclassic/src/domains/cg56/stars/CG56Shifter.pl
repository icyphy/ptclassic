defstar {
    name { Shifter }
    domain { CG56 }
    desc { Hard shifter. }
    version { @(#)CG56Shifter.pl	1.17 06 Oct 1996 }
    author { Chih-Tsung Huang, ported from Gabriel }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
	htmldoc {
<a name="bit shifter"></a>
Scale by shifting left <i>leftShifts</i> bits.
Negative values of <i>N</i> implies right shifting.
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
    state {
	    name { saturation }
	    type { string }
	    default { "YES" }
	    desc { If true, use saturation arithmetic }
    }
    constructor {
	noInternalState();
    }
    protected {
	int doSat;
    }
    setup {
	int scnt = int(leftShifts);
	absShifts = scnt >= 0 ? scnt : - scnt;
	const char *str = saturation;
	doSat = str[0]=='Y' || str[0]=='y';
    }
    codeblock(cbLoad) {
        move    $ref(input),a
    }
    codeblock(cbShifts) {
	IF	$val(absShifts)>=1
          rep     #$val(absShifts)
	ENDIF

	IF	$val(leftShifts)>0
          asl     a
	ELSE
          asr     a
	ENDIF
    }
    codeblock(cbSaveSat) {
	move	a,$ref(output)
    }
    codeblock(cbSaveNosat) {
	move	a1,$ref(output)
    }
    go {
    	addCode(cbLoad);
	if ( int(absShifts)!=0 ) {
	    addCode(cbShifts);
	}
	if ( doSat )		addCode(cbSaveSat);
	else			addCode(cbSaveNosat);
    }
    exectime {
	return int(absShifts) + 3;
    }
}



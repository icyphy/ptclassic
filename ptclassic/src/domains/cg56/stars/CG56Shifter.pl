defstar {
    name { Shifter }
    domain { CG56 }
    desc { Hard shifter. }
    version { $Id$ }
    author { Chih-Tsung Huang, ported from Gabriel }
    copyright { 1992 The Regents of the University of California }
    location { CG56 demo library }
    explanation {
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
	    attributes { A_NONSETTABLE }
    }
    state {
	    name { saturation }
	    type { string }
	    default { "YES" }
	    desc { If true, use saturation arithmetic }
    }
    protected {
	int doSat;
    }
    start {
	int scnt = int(leftShifts);
	absShifts = scnt >= 0 ? scnt : - scnt;
	const char *str = saturation;
	doSat = str[0]=='Y' || str[0]=='y';
    }
    codeblock(cbLoad) {
        move    $ref(input),a
    }
    codeblock(cbShifts) {
	IF	$val(absShifts) >= 1
          rep     #$val(absShifts)
	ENDIF

	IF	$val(leftShifts) > 0
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
    	gencode(cbLoad);
	if ( int(absShifts)!=0 ) {
	    gencode(cbShifts);
	}
	if ( doSat )		gencode(cbSaveSat);
	else			gencode(cbSaveNosat);
    }
    exectime {
	return int(absShifts) + 3;
    }
}



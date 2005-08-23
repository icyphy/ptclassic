defstar {
    name { Skew }
    domain { C50 }
    desc { Generic skewing star. }
    version { $Id$ }
    author { Luis Gutierrez, based on the CG56 version }
    copyright {
Copyright (c) 1991-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 library }
    explanation {
The \fIerror\fR input is accumulated.
Normally the constant 0.0 is output.
However, when overflow occurs, \fIoverVal\fP is output, 
and when underflow occurs, \fIunderVal\fP is output.
    }
    seealso {SSISkew}
    input {
	name { error }
	type { FIX }
    }
    output {
	name { output }
	type { FIX }
    }
    state {
	name { accum }
	type { FIX }
	default { 0.5 }
	desc { Accumulation of error skew. }
	attributes { A_NONSETTABLE|A_NONCONSTANT|A_UMEM }
    }
    state {
	name { underflowVal }
	type { FIX }
	default { "-0.9" }
	desc { Value to output on underflow. }
    }
    state {
	name { overflowVal }
	type { FIX }
	default { "0.9" }
	desc { Value to output on overflow. }
    }

    protected{
	unsigned int ioverflw, iunderflw;
	}

    codeblock(cbSkew,"") {
	setc	ovm		; set overflow mode
	clearc	sxm		; clear sign xtension mode
	lar	ar0,#$addr(error)
	lar	ar1,#$addr(accum)
	lar	ar2,#$addr(output)
	mar	*,ar2
	sst	#0,*		; ov bit is not reset so one must
	apl	#efffh,*	; clear ov bit befor accumulate
	lst	#0,*,ar0	; so that it's set correctly after add.
	lach	*,16,ar1	; accH = error
	add	*,16,ar2	; accH = error + accum
	lar	ar3,#0000h	; clear ar3
@( (ioverflw > 255) ? "\txc\t2,ov,gt\n":"\txc\t1,ov,gt\n")\
	lar	ar3,#@ioverflw
@( (iunderflw > 255) ? "\txc\t2,ov,gt\n":"\txc\t1,ov,gt\n")\
	lar	ar3,#@iunderflw
	xc	2,lt
	lacc	#8000h,15
	xc	2,ov
	lacc	#8000h,15
	smmr	ar3,*,ar1		; output value
	sach	*,0
	setc	sxm			; restore sign xtension mode
	}

    setup{
		double temp = underflowVal.asDouble();
		if (temp >= 0){
			iunderflw = int(32768*temp);
		} else {
			iunderflw = int(32768*(1-temp));
		}
		temp = overflowVal.asDouble();
		if (temp >= 0){
			ioverflw = int(32768*temp);
		} else {
			iunderflw = int(32768*(1-temp));
		}
	}
 
    go {
	addCode(cbSkew());
    }

    exectime{
	return 23;
   }
}







defstar {
    name { SSISkew }
    domain { CG56 }
    derivedFrom { SSI }
    desc { Interface to the 56001 SSI's port with timing-skew capability. }
    version { $Id$ }
    author { Kennard White }
    copyright { 1991 The Regents of the University of California }
    location { CG56 library }
    explanation {
This derived star adds an additional \fIerror\fP input used to control the
skew.  The error value is added to an internal accumulator.

When the accumulator underfows 0.0, we conceptualy slow down the timing clock.
The output (recv) sample will be a repeat,
and the input (xmit) sample will be dropped.
This is implemented by not adjusting the internal queue pointers.

When the accumulator overflow 0.0, we conceptually speed up the timing
clock.  The current output (recv) sample will be skipped and the next sample
will be returned.  The input (xmit) sample will be transmitted twice.
This is implemented by accessing the queues twice.

Note: the timing clock discussed above is with reference to the recv signal.
More sophisticated applications will probably want to control skewing
of input and output independently; this would require the SSI star to
be operating in dual-buffer mode.

When the parameter \fIinternalAccum\fP is false, it is assumed that
the \fIerror input\fP is the output from an SSISkew star.  A positive
value is treated as an overflow, and a negative value is treated
an underflow.
    }
    seealso {SSI}
    input {
	name { error }
	type { FIX }
    }
    state {
	name { internalAccum }
	type { INT }
	default { "YES" }
	desc { "Boolean: internal or external accumlator." }
    }
    defstate {
	name { accum }
	type { FIX }
	default { 0.5 }
	desc { Accumulation of error skew. }
	attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
    }
    constructor {
	;
    }

    setup {
	if ( int(queueSize) == 0 ) {
	    queueSize = 1;	// force interupts
	}
	CG56SSI::setup();
    }
    initCode {
	if ( output1.bufSize() != 1 || output2.bufSize() != 1 ) {
	    // if the bufSize is greater than 1, then subsequent firings
	    // will have the output port in different locations,
	    // and thus we cant assue the old value is already there.
	    savePrevOut = 1;
	}
	CG56SSI::initCode();
    }
    codeblock(cbSkewTop_external) {
	move	$ref(error),a
	tst	a
	.IF	<MI>			; minus (underflow 0.0)
	IF	$val(savePrevOut)
	  move	$ref(prevOut1),y0
	  move	y0,$ref(output1)
	  move	$ref(prevOut2),y0
	  move	y0,$ref(output2)
	ENDIF
	.ELSE
	jeq	$label(normal)		; ext. clear
	.LOOP	#2			; ext. set (overflow 1.0)
$label(normal)
    }
    codeblock(cbSkewTop_internal) {
	move	$ref(accum),a
	move	$ref(error),x0
	add	x0,a	#0.5,y0
	move	a1,$ref(accum)
	.IF	<MI>			; minus (underflow 0.0)
;	bchg	#0,x:m_pcd		; flip SCI RXD
	move	y0,$ref(accum)
	IF	$val(savePrevOut)
	  move	$ref(prevOut1),y0
	  move	y0,$ref(output1)
	  move	$ref(prevOut2),y0
	  move	y0,$ref(output2)
	ENDIF
	.ELSE
	jec	$label(normal)		; ext. clear
;	bchg	#1,x:m_pcd		; flip SCI TXD
	move	y0,$ref(accum)
	.LOOP	#2			; ext. set (overflow 1.0)
$label(normal)
    }
    codeblock(cbSkewBot) {
	.ENDL
	.ENDI
	nop				; required!!!
    }
    go {
	if ( int(internalAccum) ) {
	    addCode(cbSkewTop_internal);
	} else {
	    addCode(cbSkewTop_external);
	}
	CG56SSI::go();
	addCode(cbSkewBot);
    }
}

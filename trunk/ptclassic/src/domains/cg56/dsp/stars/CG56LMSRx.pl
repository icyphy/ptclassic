defstar {
    name { LMSRx }
    domain { CG56 }
    desc { Complex LMS filter. }
    version { $Id$ }
    author { Kennard White (ported from Gabriel) }
    acknowledge { Gabriel version by Steve How and Maureen O'Reilly }
    copyright { 1991 The Regents of the University of California }
    location { CG56 library }
    input {
	name { input_r }
	type { FIX }
    }
    input {
	name { input_i }
	type { FIX }
    }
    input {
	name { error_r }
	type { FIX }
    }
    input {
	name { error_i }
	type { FIX }
    }
    output {
	name { output_r }
	type { FIX }
    }
    output {
	name { output_i }
	type { FIX }
    }
    explanation {
	DSP56000 - complex lms filter star.
        The number of initial coefficients specify the order.
        	y = c' * x	c[n+1] = c[n] + (step_size)(x)(e')
	where x = input, c = coefficients, y = output,
	      e = error (desired output - actual output,
        and ' represents the complex conjugate.
	Default step size 0.01
        Error_delay must specify the total delay between
           	the filter output and the error input.
	 Last_tap_min constrains the minimum magnitude of both
		the real & imaginary parts of the last tap.
        Default coefficients give a 7th order filter, with the
        		middle real tap = 0.5 and all others zero.
    }
    state {
	name { init_taps_r }
	type { FIXARRAY }
    }
    state {
	name { init_taps_i }
	type { FIXARRAY }
    }
    state {
	name { numTaps }
	type { INT }
	default { "0" }
	attributes { A_CONSTANT|A_NONSETTABLE }
    }
    state {
	name { step_size }
	type { FIX }
	default { "0.01" }
	attributes { A_NONCONSTANT|A_SETTABLE|A_YMEM }
    }
    state {
	name { error_delay }
	type { INT }
	default { "1" }
    }
    state {
	name { decimation }
	type { INT }
	default { "1" }
    }
    state {
	name { fooPhase }
	type { INT }
	default { "0" }
	desc { "I wish I knew." }
	attributes { A_CONSTANT|A_NONSETTABLE }
    }
    state {
	name { last_tap_min }
	type { FIX }
	default { "0.0" }
    }
    state {
	name { coef_table }
	type { FIXARRAY }
	default { "0.0" }
	attributes { A_SYMMETRIC|A_RAM|A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_NOINIT }
    }
    state {
	name { delay_line }
	type { FIXARRAY }
	default { "0.0" }
	attributes { A_SYMMETRIC|A_RAM|A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_NOINIT }
    }
    state {
	name { delayPtr }
	type { INT }
	default { 0 }
	attributes { A_YMEM|A_NONCONSTANT|A_NONSETTABLE|A_NOINIT }
    }
    setup {
	int d = int(decimation);
	input_r.setSDFParams( d, d-1);
	input_i.setSDFParams( d, d-1);
	numTaps = init_taps_r.size();
	if ( numTaps <= 0 || numTaps != init_taps_i.size() ) {
	    Error::abortRun(*this,"Initial tap lists invalid");
	    return;
	}
	coef_table.resize(numTaps);
	fooPhase = int(decimation) * (int(error_delay)-1);
	delay_line.resize(numTaps+int(fooPhase));
    }
    initCode {
	addCode(cbInit);
	int k;
	addCode("	org	x:$addr(coef_table)");
	for ( k=0; k < numTaps; k++) {
	    char	buf[100];
	    sprintf(buf, "	dc	%.15f", double(init_taps_r[k]));
	    addCode(buf);
	}
	addCode("	org	y:$addr(coef_table)");
	for ( k=0; k < numTaps; k++) {
	    char	buf[100];
	    sprintf(buf, "	dc	%.15f", double(init_taps_i[k]));
	    addCode(buf);
	}
	addCode("	org	p:");
    }
    go {
	addCode(cbHeader);
	addCode(cbSetup);
	addCode(cbUpdateTaps);

	if (last_tap_min.asDouble() > 0.0) {
    	    addCode(cbLastTapConstrait);
	} else {
    	    addCode(cbStoreLastTap);
	}

	addCode(cbUpdateDelayLine);
	addCode(cbInnerProduct);
    }

    execTime {
	int coretime = 8 * int(numTaps) + 3 * int(decimation) + 28;
	if ( last_tap_min.asDouble() > 0.0 )
	    coretime += 14;
	if ( int(numTaps) <= 2 ) {
	    coretime += int(decimation) <= 1 ? 0 : 3;
	} else {
	    coretime += int(decimation) <= 1 ? 6 : 9;
	}
	return coretime;
    }

    codeblock(cbExample) {
	;	decimation (e.g. =3)
	;	time ->
	;	x0	x1	x2	|  x0	 x1	x2
	;	e	no_info	no_info	|  e	 n/i	n/i
	;			lms_out	|		lms_out
	;	note: total delay between e_in and the last x(decimation-1) =
	;	      (* error_delay  decimation)
	;	      where error_delay is delay in feedback loop
    }

    codeblock(cbInit) {
	org	$ref(delayPtr)
	dc	$addr(delay_line)
	org	x:$addr(delay_line)
	bsc	$size(delay_line),0.0
	org	y:$addr(delay_line)
	bsc	$size(delay_line),0.0
	org	p:
    }

    codeblock(cbHeader) {
	; step_size:	$val(step_size)
	; error_delay:	$val(error_delay)
	; decimation:	$val(decimation)
	; last_tap_min:	$val(last_tap_min)
    }

    codeblock(cbSetup) {
	; initialize address registers
	move	#$val(numTaps)-1+$val(fooPhase),m1 ; delay_line: oldest sample
	move	$ref(delayPtr),r1
	move	#-(1+$val(fooPhase)),n1
	move	#$val(numTaps)-1,m4
	move	#$addr(coef_table),r4			; coef_table
    }

    codeblock(cbUpdateTaps) {
	; UPDATE TAPS
	lua	(r1)+n1,r1
	move	$ref(error_i),x1
	move	$ref(step_size),y1
	mpyr	x1,y1,b
	move	$ref(error_r),x1
	mpyr	x1,y1,a		b,y1		x:(r1),x0
	move			a,x1		y:(r4),b
        ; Now (error * step_size) =  x1 + jy1

        ; Do re(updated_coef)-->a    im(updated_coef)-->b
	mac	-x0,y1,b			y:(r1)-,y0
	macr	 x1,y0,b	x:(r4),a

	; going into loop - (accounting for decimation)
	;	x0+jy0	=	in@total_error_delay
	;	a	=	re(old_coef0)
	;	b	=	im(updated_coef0)
	;	r4	->	coef0
	;	r1	->	in@(total_error_delay+1)

	IF $val(numTaps)>2
	  .LOOP	#$val(numTaps)-1
	    mac		 x0,x1,a	b,y:(r4)+
	    macr	 y0,y1,a	y:(r4)-,b	x:(r1),x0
	    mac		-x0,y1,b	a,x:(r4)+	y:(r1)-,y0
	    macr	 x1,y0,b	x:(r4),a
	  .ENDL
	ENDIF

	; coming out of loop -
	;	b	=	im(updated_coef_N-1)
	;	x0+jy0	=	in@error_delay
	;	r1	->	messed-up
	;	r4	->	coef_N-1
    }

    codeblock(cbStoreLastTap) {
	mac	 x0,x1,a	b,y:(r4)
	macr	 y0,y1,a
	move			a,x:(r4)+
    }
	
    codeblock(cbLastTapConstrait) {
	; update last tap only if its magnitude exceeds last_tap_min
	; last_tap_min --> x1
	; re(updated_coef)-->x0, im(updated_coef)-->y0
	mac	x0,x1,a		#$val(last_tap_min),x1
	macr	y0,y1,a		b,y0
	abs	b		a,x0
	cmp	x1,b
	jlt	$label(small_imag)	;jump if im(updated_coef) too small
	move	y0,y:(r4)
$label(small_imag)
	abs	a
	cmp	x1,a
	jlt	$label(small_real)	;jump if re(updated_coef) too small
	move	x0,x:(r4)
$label(small_real)
	lua	(r4)+,r4
    }

    codeblock(cbUpdateDelayLine) {
	; UPDATE DELAY LINE
	; MOVE CURRENT INPUTS INTO DELAY_LINE
	move	$ref(delayPtr),r1
	move	#$addr(input_r),r0
	move	#$addr(input_i),r5
	IF	$val(decimation)>1
	  .LOOP	#$val(decimation)
	ENDIF
	  move	x:(r0)+,x1
	  move	x:(r5)+,x0
	  move	x,l:(r1)+
	IF	$val(decimation)>1
	  .ENDL
	ENDIF

	; update delay_line pointer
	move	r1,$ref(delayPtr)
    }

    codeblock(cbInnerProduct) {
	; COMPUTE INNER PRODUCT
        ; COMPUTE FILTER OUTPUT
	lua	(r1)-,r1
	nop
	clr	a		y:(r4),y1	x:(r1),x0
	clr	b		x:(r4)+,x1	y:(r1)-,y0
	mac	-y1,x0,b
	mac	 x1,y0,b

	; going into loop -
	;	x1+jy1	=	coef0
	;	x0+jy0	=	in@0 (first new sample)
	;	r1	->	in@-1
	;	r4	->	coef1

	IF	$val(numTaps)>2
	  .LOOP	#$val(numTaps)-1
	ENDIF
	  mac	x0,x1,a
	  mac	y0,y1,a		y:(r4),y1	x:(r1),x0
	  mac	-y1,x0,b	x:(r4)+,x1	y:(r1)-,y0
	  mac	x1,y0,b
	IF	$val(numTaps)>2
	  .ENDL
	ENDIF

	mac	x0,x1,a		b,$ref(output_i)
	mac	y0,y1,a
	move	a,$ref(output_r)
	move	m7,m1
	move	m7,m4
   }
}

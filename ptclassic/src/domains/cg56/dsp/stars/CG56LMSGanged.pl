defstar {
	name { LMSGanged }
	domain { CG56 }
	desc { Ganged least mean square (LMS) adaptive filter. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	acknowledge { Gabriel version by Martha Fratt }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
        explanation {
Coefficients from the adaptive filter are used for the FIR filter in parallel.
The order is determined from the number of initial coefficients.
Initial coefficients default to a lowpass filter of order 8.
Default stepSize 0.01.
The errorDelay must specify the total delay between the
filter output and the error input.
	}
        
        input  {
                name { inputFIR }
	        type { fix }
	}
        input  {
                name { inputAdapt }
	        type { fix }
	}
        input  {
                name { error }
	        type { fix }
	}
        output {
		name { outputFIR }
		type { fix }
	}
        output {
		name { outputAdapt }
		type { fix }
	}
        state  {
                name { stepSize }
                type { fix }
                default { 0.01 }
                desc { specifies the rate of adaptation }
        }
	state {
		name { errorDelay }
		type { int }
		desc {
specifies the relative delay between the output samples and the input
error samples.
                }
		default { 1 }
	}
	state {
// currently not supported 
		name { decimation }
		type { int }
		desc { number of input samples consumed before firing. }
		default { 1 }
                attributes { A_NONSETTABLE }
	}
        state {
                name { coef }
                type { fixarray }
                desc { internal }
                default { "-4.0609e-2 -1.6280e-3 1.7853e-1 3.7665e-1 3.7665e-1 1.7853e-1 -1.6280e-3 -4.0609e-2" }
                attributes { A_NONCONSTANT|A_XMEM }
        }
        state {
                name { coefLen }
                type { int }
                desc { number of coef. }
                default { 8 }
                attributes { A_NONSETTABLE|A_NONCONSTANT }
        }        

        state {
                name { delayLineAdp }
                type { intarray }
                desc { internal }
                default { 0 }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
        }
        state {
                name { delayLineAdpStart }
	        type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
	}
        state {
                name { delayLineFIR }
                type { intarray }
                desc { internal }
                default { 0 }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
        }
        state {
                name { delayLineFIRStart }
	        type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
	}
        state {
                name { delayLineAdpSize }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
        state {
                name { delayLineFIRSize }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

       
        codeblock(init) {
; delayLineAdp memory
        org     $ref(delayLineAdp)
        bsc     $val(delayLineAdpSize),0
        org     p:

; pointer to adapt delay line into memory
        org     $ref(delayLineAdpStart)
        dc      $addr(delayLineAdp)
        org     p:
    
; delayLineFIR memory
        org     $ref(delayLineFIR)
        bsc     $val(delayLineFIRSize),0
        org     p:
      
; pointer to FIR delay line into memory
        org     $ref(delayLineFIRStart)
        dc      $addr(delayLineFIR)
        org     p:
        }

        codeblock(first) {
        move    #$val(coefLen)+$val(errorDelay)-2,m5
        }
        codeblock(errorDelayGtOne) {
        move    #$val(errorDelay)-1,n5
        }
        codeblock(main) {
        move    #$val(stepSize),x1
        move    $ref(error),x0
; load the address of the oldest sample into r5
        move    $ref(delayLineAdpStart),r5
; load the address of the last coefficient into r3
        move    #$addr(coef)+$val(coefLen)-1,r3
; multiply the error by the stepSize
        mpyr    x0,x1,a         y:(r5)+,y0
        move    a,x0
        move    x:(r3),b
        }
        codeblock(coefLenTwo) {
        do      #$val(coefLen)-2,$label(loop)
        macr    x0,y0,b
        move    b,x:(r3)-
        move    x:(r3),b        y:(r5)+,y0
$label(loop)
        move    $ref(coef),a
        macr    x0,y0,b         y:(r5)+,y0
        }
        codeblock(std) {
        macr    x0,y0,b
        move    b,x:(r3)-
        move    x:(r3),b        y:(r5)+,y0
        move    $ref(coef),a
        macr    x0,y0,b         y:(r5)+,y0
        }    
        codeblock(adjust) {
        move    (r5)+n5         ; to handle error delay
        }
        codeblock(cont) {
        move    $ref(inputAdapt),y1        ;fetch input
        macr    x0,y0,a         b,x:(r3)-
        move    a,x:(r3)        y1,y:(r5)+
        move    r5,$ref(delayLineAdpStart) ;oldest sample pointer
; now compute output.
        }    
        codeblock(cont1) {
        move    #$addr(coef)+$val(coefLen)-1,r3
        clr     a
        move    x:(r3)-,x0      y:(r5)+,y0
        do      #$val(coefLen)-1,$label(loop1)
        mac     x0,y0,a         x:(r3)-,x0      y:(r5)+,y0
$label(loop1)
        macr    x0,y0,a
        move    a,$ref(outputAdapt)
; compute output of parallel fir filter
        move    #$val(coefLen)-2,m5
        move    $ref(delayLineFIRStart),r5
        move    #$addr(coef)+$val(coefLen)-1,r3
        clr     a
        move    x:(r3)-,x0      y:(r5)+,y0
        do      #$val(coefLen)-2,$label(loop3)
        mac     x0,y0,a         x:(r3)-,x0      y:(r5)+,y0
$label(loop3)
        move    $ref(inputFIR),y1
        macr    x0,y0,a         x:(r3),x0
        macr    x0,y1,a         y1,y:(r5)+
        move    a,$ref(outputFIR)
        move    r5,$ref(delayLineFIRStart)
        move    m7,m5
        }

        start {
                coefLen=coef.size();
                delayLineAdpSize=errorDelay-1+coefLen;
		delayLineAdp.resize(delayLineAdpSize);

                delayLineFIRSize=coefLen-1;
		delayLineFIR.resize(delayLineFIRSize);
        }
        initCode  {
	        addCode(init);
        }
        go { 
                addCode(first);
		if (errorDelay>1) addCode(errorDelayGtOne);
                addCode(main);
		if (coefLen>2)
	             addCode(coefLenTwo);
		else
	             addCode(std);
// need to adjust r5 if errorDelay is greater than one.
		if (errorDelay>1) addCode(adjust);
                addCode(cont);
// assume r5 is pointing to the oldest sample in the delay line
// but if errorDelay is greater than one, we don't want to point
// to the oldest		
		if (errorDelay>1) addCode(adjust);
// make r3 point to the last tap value
                addCode(cont1);
        }             

	execTime { 
              if(int(errorDelay)>1)
	           return 33+5 * int(coefLen);
              else
                   return 30+5 * int(coefLen);
        }
}   


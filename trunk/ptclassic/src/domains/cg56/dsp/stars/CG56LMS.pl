defstar {
	name { LMS }
	domain { CG56 }
	desc { Least mean square (LMS) adaptive filter. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
DSP56000 - Least Mean Square Adaptive Filter Star
           The order is determined from the number of initial coefficients.
           Initial coefficients default to a lowpass filter of order 8.
           Default stepSize 0.01
           The errorDelay must specify the total delay between
           the filter output and the error input.
	}
        
        output {
		name { output }
		type { fix }
	}
        input  {
                name { input }
	        type { fix }
	}
        input  {
                name { error }
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
		name { decimation }
		type { int }
		desc { number of input samples consumed before firing. }
		default { 1 }
	}
        state {
                name { coef }
                type { fixarray }
                desc { internal }
                default { "-4.0609e-2 -1.6280e-3 1.7853e-1 3.7665e-1 3.7665e-1 1.7853e-1 -1.6280e-3 -4.0609e-2" }
                attributes { A_XMEM }
        }
        state {
                name { coefLen }
                type { int }
                desc { number of coef. }
                default { 8 }
                attributes { A_NONSETTABLE|A_NONCONSTANT }
        }        

        state {
                name { Y }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONSETTABLE|A_NONCONSTANT }
        }        

        state {
                name { loopVal }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONSETTABLE|A_NONCONSTANT }
        }            

        state {
                name { delayLine }
                type { intarray }
                desc { internal }
                default { 0 }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT}
        }
        state {
                name { delayLineStart }
	        type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
	}
        state {
                name { delayLineSize }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

        codeblock(std) {
	; initialize address registers for coef and delayLine
         move    #$addr(coef)+$val(coefLen)-1,r3
; insert here
        move    $ref(delayLineStart),r5            ; delayLine
        move    #$val(Y),m5
                                  ; first adapt coefficients.
                                  ; multiply the error by the stepSize --> x0
        move    #$val(stepSize),x1
        move    $ref(error),x0
        mpyr    x0,x1,a
        move    a,x0
        move    x:(r3),b        y:(r5)+,y0
        }

        codeblock(loop) {
        do      #$val(loopVal),$label(endloop)
        macr    x0,y0,b
        move    b,x:(r3)-
        move    x:(r3),b        y:(r5)+,y0
$label(endloop)
        }

        codeblock(noloop) {
        macr    x0,y0,b
        move    b,x:(r3)-
        move    x:(r3),b        y:(r5)+,y0
        }
        codeblock(cont) {
        macr    x0,y0,b
        move    b,x:(r3)
; move current inputs into delayLine.
        move    #$addr(input),r0
        move    $ref(delayLineStart),r5
        }

        codeblock(decimationGreaterthanOne) {
        do      #$val(decimation),$label(decimationloop)
        move    x:(r0)+,y1
        move    y1,y:(r5)+
$label(decimationloop)
        }

        codeblock(decimationOne) {
        move    x:(r0)+,y1
        move    y1,y:(r5)+
        }

        codeblock(cont1) {
; update delayLine pointer.
        move    r5,$ref(delayLineStart)          ;oldest sample pointer
                                            ; now compute output.
        lua     (r5)-,r5
        nop
        clr     a               x:(r3)+,x0      y:(r5)-,y0
        }

        codeblock(loop1) {
        do      #$val(loopVal),$label(loop1)
        mac     x0,y0,a         x:(r3)+,x0      y:(r5)-,y0
$label(loop1)
        }

        codeblock(noloop1) {
        mac     x0,y0,a         x:(r3)+,x0      y:(r5)-,y0
        }

        codeblock(end) {
        macr    x0,y0,a
        move    a,$ref(output)
        move    m7,m5
        }
       
        codeblock(makeblock) {
; delayLine memory
        org     $ref(delayLine)
        bsc     $val(delayLineSize),0
        org     p:
        }
      
        codeblock(delaystart) {
; pointer to delay line into memory
        org     $ref(delayLineStart)
        dc      $addr(delayLine)
        org     p:
        }
        start {
                coefLen=coef.size();
                delayLineSize=errorDelay-1;
		delayLineSize=coefLen+decimation*delayLineSize;
		delayLine.resize(delayLineSize);
                input.setSDFParams(int(decimation),int(decimation)-1);

	        if (decimation <=0)
        	      Error::abortRun(*this, "Decimation must be greater than 0.");

        }
        initCode  {
	        gencode(makeblock);
                gencode(delaystart);
        }
        go { 
                Y=errorDelay-1;
                Y=coefLen-1+decimation*Y;

	        gencode(std);
	
        	if(coefLen>2) {
	            loopVal=coefLen-1;    
	            gencode(loop);
		}
		else gencode(noloop);

	        gencode(cont);
      	
		if(decimation>1)
	            gencode(decimationGreaterthanOne);
		else
	            gencode(decimationOne);
	    
	        gencode(cont1);

		if(coefLen>2)
        	     gencode(loop1);
		else
        	     gencode(noloop1);
	     
	        gencode(end);
        }             
	execTime { 
              if (int(coefLen)<=2) {
                   if (int(decimation)<=1)
                         return (17+int(coefLen)*4+int(decimation)*2);
                   else
                         return (20+int(coefLen)*4+int(decimation)*2);
              }
              else {
                   if (int(decimation) <=1)
                         return (23+int(coefLen)*4+int(decimation)*2);
                   else
           	         return (26+int(coefLen)*4+int(decimation)*2);
	      }
        }
}

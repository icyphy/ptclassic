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
        
        input  {
                name { input }
	        type { fix }
	}
        input  {
                name { error }
	        type { fix }
	}
        output {
		name { output }
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
                attributes { A_ROM|A_NONCONSTANT|A_XMEM }
        }
        state {
                name { coefLen }
                type { int }
                desc { number of coef. }
                default { 8 }
        }        

        state {
                name { X }
                type { int }
                desc { internal }
                default { 0 }
        }        
    
        state {
                name { Y }
                type { int }
                desc { internal }
                default { 0 }
        }        

        state {
                name { loopVal }
                type { int }
                desc { internal }
                default { 0 }
        }            

        state {
                name { delayLine }
                type { int }
                desc { internal }
                default { 1 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
        }
        state {
                name { delayLineSize }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

        state  {
                name { runtimeVal }
                type { int }
                default { 0 }
                desc { runtime value }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
        codeblock(std) {
	; initialize address registers for coef and delayLine
        move    #$val(X),r3     	      ; coef
        move    $ref(delayLine),r5            ; delayLine
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
        move    $ref(delayLine),r5
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
        move    r5,$ref(delayLine)          ;oldest sample pointer
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
        org     $ref(delayLine)
        bsc     $val(coefLen),0
        org     p:
        }

        start {
                input.setSDFParams(int(decimation),int(decimation)-1);

	        if (decimation <=0)
        	      Error::abortRun(*this, "Decimation must be greater than 0.");
        // initial tap values.
//                delayLineSize=decimation*(errorDelay-1)+coefLen;
        }
    
        initCode  {
	        gencode(makeblock);
//        	delayLine=&delayLine;
        }
        go { 
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

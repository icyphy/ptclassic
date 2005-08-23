defstar {
	name { LMS }
	domain { CG56 }
	desc { Least mean square (LMS) adaptive filter. }
	version { @(#)CG56LMS.pl	1.17 03 Oct 1996 }
	acknowledge { Gabriel version by E. A. Lee, Maureen O'Reilly }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	htmldoc {
<p>
This star implements an adaptive filter using the LMS algorithm.
The initial coefficients determine the order of the filter.
The default coefficients specify
an eighth order, equiripple, linear-phase, lowpass filter,
the same as the default coefficients of the <i>FIR</i> star.
<p>
The <i>stepSize</i> parameter specifies the rate of adaptation.
<p>
The <i>errorDelay</i> parameter specifies the relative delay between the output
samples and the input error samples.  There must be at least
a delay of one (you must add the delay in your system) because
the path from the output to the error forms a closed feedback loop.
You can insert more delays if you wish (you may have to decrease
<i>stepSize</i> to keep the algorithm stable), but be sure to
adjust the <i>errorDelay</i> parameter accordingly.
<p>
The <i>decimation</i> parameter specifies how many input samples should be
consumed before firing the star.  For example, to downsample
by a factor of two, the user could simply follow the LMS filter by
a downsample star, which would be set to discard every second input.
But it is inefficient for the LMS filter to compute these samples,
only to have them discarded.  Setting the <i>decimation</i> parameter to 2
in the LMS filter is exactly equivalent,
except that the discarded samples are not computed.
Interpolation is not supported.
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
        setup {
		// FIXME: Parameters are not always resolved properly
		// before setup but should be.  For now, check parameters
		// in initCode method.
		// CheckParameterValues();

		coefLen = coef.size();
		delayLineSize = int(errorDelay) - 1;
		delayLineSize = int(coefLen) +
				int(decimation)*int(delayLineSize);
		delayLine.resize(int(delayLineSize));
		input.setSDFParams(int(decimation),int(decimation)-1);
        }
        method {
	    name {CheckParameterValues}
	    arglist { "()" }
	    type { void }
	    code {
	        if (int(decimation) <= 0) {
        	    Error::abortRun(*this,
				    "Decimation must be greater than 0.");
		    return;
		}
	    }
	}
        initCode  {
		CheckParameterValues();
	        addCode(makeblock);
                addCode(delaystart);
        }
        go { 
                Y = int(errorDelay)-1;
                Y = int(coefLen) - 1 + int(decimation)*int(Y);

	        addCode(std);

        	if (int(coefLen) > 2) {
	            loopVal = int(coefLen) - 1;    
	            addCode(loop);
		}
		else addCode(noloop);

	        addCode(cont);
      	
		if ( int(decimation) > 1)
	            addCode(decimationGreaterthanOne);
		else
	            addCode(decimationOne);
	    
	        addCode(cont1);

		if ( int(coefLen) > 2)
        	     addCode(loop1);
		else
        	     addCode(noloop1);
	     
	        addCode(end);
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

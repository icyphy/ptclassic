defstar {
	name { LMS }
	domain { C50 }
	desc { Least mean square (LMS) adaptive filter. }
	version { $Id$ }
	acknowledge { Gabriel version by E. A. Lee, Maureen O'Reilly }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
        explanation {
.pp
This star implements an adaptive filter using the LMS algorithm.
The initial coefficients determine the order of the filter.
The default coefficients specify
an eighth order, equiripple, linear-phase, lowpass filter,
the same as the default coefficients of the \fIFIR\fP star.
.PP
The \fIstepSize\fP parameter specifies the rate of adaptation.
.PP
The \fIerrorDelay\fP parameter specifies the relative delay between the output
samples and the input error samples.  There must be at least
a delay of one (you must add the delay in your system) because
the path from the output to the error forms a closed feedback loop.
You can insert more delays if you wish (you may have to decrease
\fIstepSize\fP to keep the algorithm stable), but be sure to
adjust the \fIerrorDelay\fP parameter accordingly.
.PP
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
		attributes { A_NONCONSTANT|A_UMEM }
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
                name { coef }
                type { fixarray }
                desc { internal }
                default { "-4.0609e-2 -1.6280e-3 1.7853e-1 3.7665e-1 3.7665e-1 1.7853e-1 -1.6280e-3 -4.0609e-2" }
                attributes { A_NONCONSTANT|A_UMEM }
        }
        state {
                name { coefLen }
                type { int }
                desc { number of coef. }
                default { 8 }
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
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT}
        }
        state {
                name { delayLineStart }
	        type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
	}
        state {
                name { delayLineSize }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }   
	 state {
                name { delayLineSpace }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

        codeblock(std) {
	mar	*,AR4
        lar	AR0,#$addr(error)		;Address error		=> AR0
	lar	AR1,#$addr(stepSize)		;Address stepSize	=>AR1
	lar    	AR3,#$addr(coef)		;Address coef		=>AR3
	lar    	AR4,#$addr(delayLineStart)	;Address Delay Pointer	=>AR4
	lar	AR5,*,AR0 			;Delay Pointer		=>AR5
	lar	AR7,#$addr(output)          	;Address output		=>AR7
	lt	*,AR1				;TREG0 = error
	mpy    	*,AR3				;error*stepSize
	sph	TREG0				;TREG0=error*stepSize
	lacc	*,15,AR5			;Accu = coef(0)
        }

	codeblock(loop) {
	splk	#$addr(delayLineStart),CBSR1	;Startadress circ. buf. 1
	splk	#$addr(delayLineStart)+$val(delayLineSize)-1,CBER1
	splk	#0dh,CBCR			;enable circ. buf 1 with AR5
        splk    #$val(loopVal),BRCR		;number of loops => BRCR
	rptb	$label(end)			;loop to label(end)
	 mpya	*+,AR3				;error*stepSize*val(k-i)
	 apac					;Accu=coef(i)+er*stSi*val(k-i)
	 sach	*+,1				;Accu => new coef(i) incr AR3
	 lacc	*,15,AR5			;Accu = coef(i+1)
$label(end)
        }

        codeblock(noloop) {
        mpya	*+,AR3				;error*stepSize*val(1) incr AR5
	apac					;Accu=coef(0)+er*stSi*val(1)
	sach	*+,1				;Accu => new coef(0)   incr AR3
	lacc	*,15,AR5			;Accu = coef(1)
        }
        codeblock(cont) {
        mpya	*,AR3				;error*stepSize*val(k)
	apac					;Accu=coef(i+1)+er*stSi*val(k)
	sach	*,1,AR5				;Accu => new coef(i+1)
	lmmr	BMAR,#$addr(input)		;Address input => BMAR
	lmmr	AR5,#$addr(delayLineStart)	;Address delayLine Pointer AR5
        bldd	BMAR,*,AR4			;input => newest delay value
        sar	AR5,*,AR5			;update delayLine pointer.
	zap					;clear P-Reg. and Accu
	splk	#$addr(coef),BMAR		;BMAR= address first coef	       }

        codeblock(loop1) {
        rpt     #$val(loopVal)			;
         mads	*+				;accu = Sum[coef(i)*u(k-i)]
        }

        codeblock(noloop1) {
        mads  	* 				;coef(1)*u(1)
	}

        codeblock(end) {
        apac					;last addition
	mar	*,AR7
        sach	*,1				;result => output
        apl	#0fff7h,CBCR			;disable circ. buffer 1
	}
       
        codeblock(makeblock) {
        .ds  	$addr(delayLine)		;delayLine memory
        .space	$val(delayLineSpace)
        .ds	$addr(delayLineStart)		;pointer to delayLine memory
        .word	$addr(delayLine)
        .text
        }
        setup {
                coefLen=coef.size();
                delayLineSize=coefLen+errorDelay-1;
		delayLine.resize(delayLineSize);
                }
        initCode  {
		delayLineSpace=int(delayLineSize)*16;
	        addCode(makeblock);
        }
        go { 
	        addCode(std);
	
        	if(coefLen>1) {
	            loopVal=coefLen-1;    
	            addCode(loop);
		}
		else addCode(noloop);

	        addCode(cont);
              
		if(coefLen>1) { 
		     loopVal=loopVal-1;	
        	     addCode(loop1);
		 }     
		else
        	     addCode(noloop1);
	     
	        addCode(end);
        }             
	execTime { 
              if (int(coefLen)<=1) {
                  return 32;
                 }
              else {
                  return (29+int(coefLen)*6);
		 }
	      return 0;	
        }
}

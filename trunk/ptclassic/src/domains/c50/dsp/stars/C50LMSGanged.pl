defstar {
	name { LMSGanged }
	domain { C50 }
	desc { Ganged least mean square (LMS) adaptive filter. }
	version {@(#)C50LMSGanged.pl	1.4	05/26/98}
	author { Chih-Tsung Huang, ported from Gabriel, G. Arslan }
	acknowledge { Gabriel version by Martha Fratt }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
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
                name { coef }
                type { fixarray }
                desc { internal }
                default { "-4.0609e-2 -1.6280e-3 1.7853e-1 3.7665e-1 3.7665e-1 1.7853e-1 -1.6280e-3 -4.0609e-2" }
                attributes { A_NONCONSTANT|A_BMEM }
        }
        state {
                name { delayLineAdp }
                type { intarray }
                desc { internal }
                default { 0 }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
        }
	
        state {
                name { delayLineAdpStart }
	        type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
	}
        state {
                name { delayLineFIR }
                type { intarray }
                desc { internal }
                default { 0 }
                attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
        }
	
        state {
                name { delayLineFIRStart }
	        type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
	}
        codeblock(init) {
          mar *,ar0
	  lar ar0,#$addr(delayLineAdp)         
	  sar ar0,$addr(delayLineAdpStart)
          lar ar0,#$addr(delayLineFIR)
	  sar ar0,$addr(delayLineFIRStart)				     
        }

        codeblock(first) {
	mar	*,AR1
        lar	AR0,#$addr(inputFIR)		;Address inputFIR 	=> AR0
	lar	AR1,#$addr(error)		;Address error	  	=> AR1
	lar	AR6,#$addr(delayLineFIRStart)	;Address FIRStart 	=> AR6
	lar	AR7,#$addr(outputAdapt)		;Address output   	=> AR7
        }
        codeblock(errorDelayGtOne) {
        splk    #$val(errorDelay)-1,INDX	;INDX=errorDelay-1
        }
        codeblock(main) {
        lt	*,AR5				;TREG0=error
        lar     AR5,#$addr(delayLineAdpStart)	;Address delayStart	=> AR5
        lar     AR3,#$addr(coef)		;Address first coef	=> AR3
        mpy     #$valfix(stepSize)        		;error*stepSize
        sph     TREG0				;TREG0=error*stepSize
        lacc    *,15,AR5			;Accu=coef(0)
        }
        codeblock(coefLenTwo) {
	splk	#$addr(delayLineAdp),CBSR1	;startadress circ. buffer 1
	splk	#$addr(delayLineAdp)+$size(delayLineAdp)-1,CBER1
	splk	#0dh,CBCR			;enable circ. buf. 1 with AR5
        lar	AR5,*				;circular buffer adress
	splk    #$size(coef)-2,BRCR		;number of loops into BRCR
	rptb	$label(loop)			;loop to label(loop)
         mpya   *+,AR3				;error*stepSize*u(k-i)
	 apac					;Accu=coef(i)+er*stSi*u(k-i)
	 sach   *+,1	 			;Accu => new coef(i)
	 lacc	*,15,AR5			;Accu=coef(i+1)
$label(loop)
        mpya	*+				;error*stepSize*u(k-N)
        apac					;Accu=coef(k)+er*stSi*u(k-N)
        }
        codeblock(std) {
        mpya    *,AR3				;error*StepSize*u(2)
	apac					;Accu=coef(0)+er*stSi*u(2)
	sach	*+,1				;Accu => new coef(0)
	lacc	*,15,AR5			;Accu=coef(1)
	mpya	*+				;error*stepSize*u(1)
	apac					;Accu=coef(1)+er*stSi*u(1)
        }    
        codeblock(adjust) {
        adrk    #$val(errorDelay)        	;AR5 = AR5 + errorDelay
        }
        codeblock(cont) {
        mar	*,AR3				;
	sach	*+,1				;Accu => new coef(N-1)
	lacc	*,15,AR5			;Accu = coef(N)
	mpya	*+,AR3				;error*stepSize*u(N)
	apac					;Accu=coef(N)+er*stSi*u(N)
	sach	*,1,AR5				;Accu => new coef(N)
	bldd	#$addr(inputAdapt),*+		;inputAdapt = newest del.value 
	sar	AR5,#$addr(delayLineAdpStart)	;store new start pointer
        }    
        codeblock(cont1) {
        lar     AR3,#$addr(coef)		;adress first coef	=> AR3
        zap					;clear P-Reg and Accu
	mar	*,AR3				;
        splk    #$size(coef)-1,BRCR		;number of loops into BRCR
	rptb	$label(lp1)			;loop to label(lp1)
	 lt	*+,AR5				;TREG0 = coef(i)
	 mpya	*+,AR3 				;P-Reg. = coef(i)*u(N-i)
$label(lp1)					;Accu = Accu + P-Reg.
        mpya	*,AR7				;P-Reg. = coef(N)*u(0)
	apac					;Accu = Accu + P-Reg.
	sach	*,1,AR6				;store Accu into output
;
; compute output of parallel fir filter
;
	splk	#$addr(delayLineFIR),CBSR1	;startadress for circ. buf. 1
	splk	#$addr(delayLineFIR)+$size(delayLineFIR)-1,CBER1
	splk	#0dh,CBCR			;enable circ. buf.1 with AR5
        lar     AR5,*,AR3			;circular buffer address
        lar     AR3,#$addr(coef)		;Adress first coef	=>AR3
        zap					;clear P-Reg. and Accu
        splk    #$size(coef)-2,BRCR		;number of loops into BRCR
	rptb	$label(lp3)			;loop to label(lp3)
	 lt	*+,AR5				;TREG0 = coef(i)	
	 mpya	*+,AR3				;P-Reg. = coef(i)*u(N-i)
$label(lp3)					;Accu = Accu + P-Reg.
	mpya	*+,AR0				;P-Reg=coef(N-1)*u(1), accumul.
	lt	*,AR3				;TREG0 = inputFIR
	mpya	*,AR5				;P-Reg = inputFIR*coef(N)
	apac					;Accu = Accu + P-Reg
	bldd	#$addr(inputFIR),*+,AR7		;inputFIR = new delayFIR value
	sach	*,1,AR6				;store Accu into output
	sar	AR5,*				;store new delayFIR pointer
        }

        setup {
		delayLineAdp.resize(errorDelay - 1 + coef.size());
		delayLineFIR.resize(coef.size()-1);
        }
        initCode  {
	        addCode(init);
        }
        go { 
                addCode(first);
		if (errorDelay>1) addCode(errorDelayGtOne);
                addCode(main);
		if (coef.size()>2)
	             addCode(coefLenTwo);
		else
	             addCode(std);
// need to adjust AR5 if errorDelay is greater than one.
		if (errorDelay>1) addCode(adjust);
                addCode(cont);
// assume AR5 is pointing to the oldest sample in the delay line
// but if errorDelay is greater than one, we don't want to point
// to the oldest		
		if (errorDelay>1) addCode(adjust);
// make AR3 point to the last tap value
                addCode(cont1);
        }             

	execTime { 
              if(int(errorDelay)>1)
	           return 44+4 * coef.size();
              else
                   return 41+4 * coef.size();
        }
}   


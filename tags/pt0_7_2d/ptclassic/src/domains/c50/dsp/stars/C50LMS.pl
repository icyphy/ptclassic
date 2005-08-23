defstar {
	name { LMS }
	domain { C50 }
	desc { Least mean square (LMS) adaptive filter. }
	version {@(#)C50LMS.pl	1.12	09/21/99}
	acknowledge { Gabriel version by E. A. Lee, Maureen O'Reilly }
	author { A. Baensch, ported from Gabriel, G. Arslan }
	copyright {
Copyright (c) 1990-1999 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
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
The <i>errorDelay</i> parameter specifies the relative delay between the
output samples and the input error samples.  There must be at least
a delay of one (you must add the delay in your system) because
the path from the output to the error forms a closed feedback loop.
You can insert more delays if you wish (you may have to decrease
<i>stepSize</i> to keep the algorithm stable), but be sure to
adjust the <i>errorDelay</i> parameter accordingly.
<p>
	}
        
	// FIXME: Should be derived from the FIR star
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
		attributes { A_CONSTANT|A_UMEM }
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
                attributes { A_NONCONSTANT | A_NONSETTABLE | A_BMEM }
        }

        state {
                name { delayLine }
                type { fixarray }
                desc { internal }
                default { 0 }
	        attributes {A_CIRC|A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
        }

        state {
                name { delayLinePtr }
	        type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
	}


	protected {

		int	numTaps;
		int	delayMask;
		int	muAsInt;
		int	length;
		StringList cfs;
	}


        // sprintf()
        ccinclude { <stdio.h> }

	setup{
		delayMask = -1;
		numTaps = coef.size();
		length = int(errorDelay)*int(decimation);
		if (length == 1) {
			length += numTaps;
		} else {
			length += numTaps;
			int bufSize = 1;
			for (int j = 1; j<16; j++){
				bufSize = 2*bufSize;
				if (bufSize >= length) break;
			}
			length = bufSize;
			delayMask = bufSize - 1;
		}
		delayLine.resize(length);

		double temp; 
		temp = stepSize.asDouble();
		if (temp >= 0) {
			muAsInt = int(temp*double(32768) + 0.5);
		} else {
			muAsInt = int((2+temp)*double(32768) + 0.5);
		}
		
		char buf[32];
		cfs.initialize();
		int i;
		for (i = 0; i< numTaps-1; i++){
			temp = double(coef[i]);
			sprintf(buf,"%.15f",temp);
			cfs << buf << " ";
		}
		temp = double(coef[i]);
		sprintf(buf,"%.15f",temp);
		cfs << buf;
		
		
	}

	initCode{
		addCode(initDelayLinePtr);
		coef.setInitValue(cfs);
	}

	codeblock(initDelayLinePtr){
        mar *,ar0
        lar ar0,#$addr(delayLine)
        sar ar0,$addr(delayLinePtr)
	}

	codeblock(prepareDelayLine,""){
*first update delay line
	lmmr	ar1,#$addr(delayLinePtr)
	ldp	#0h
	mar	*,ar1
	splk	#$addr(delayLine),cbsr1
	splk	#($addr(delayLine)+@(int(length)-1)),cber1
	splk	#09h,cbcr
	.if	@(int(decimation)-1)
	rpt	#@(int(decimation) -1)
	.endif
	bldd	#($addr(input)+@(int(decimation)-1)),*+
	smmr	ar1,#$addr(delayLinePtr)
	lmmr	treg0,#$addr(error)
	splk	#@(int(numTaps)-2),brcr
	lar	ar3,#$addr(coef)
	spm	1
	setc	ovm
	}

	codeblock(getUpdateAddress,""){
*figure out what past samples to use to update taps
*this gets called only if errorDelay*decimation > 1
*after this, ar2 -> x(n) and  ar1 -> x(n-errorDelay*decimation-numTaps-1)
	mar	*-
	lamm	ar1
	samm	ar2
	sub	#@(int(errorDelay)*int(decimation)  - int(numTaps) - 1),0
	and	#@(int(delayMask))
	or	#$addr(delayLine)
	samm	ar1
	}

	codeblock(doUpdate,""){
	mpy	#@(int(muAsInt))
	pac	
	sach	treg0,1	
	mpy	*+,ar3		
	.if	@(int(numTaps)-1)
	rptb	$label(lmsu)
	zalr	*,ar1
	mpya	*+,ar3
$label(lmsu)
	sach	*-	
	.endif
	zalr	*
	apac
	sach	*,0,ar1
	splk	#$addr(delayLine),cber1		
	splk	#($addr(delayLine)+@(int(length)-1)),cbsr1	
	zap
	}

	codeblock(prepareFIR,""){
	lar	ar1,12h
	}

	codeblock(doFIR,""){
	.if	@(int(numTaps)-1)
	rpt	#@(int(numTaps)-1)
	.endif
	mac	$addr(coef),*-
	apac
	lar	ar1,#$addr(output)
	sach	*
	splk	#0,cbcr
	spm	0
	clrc	ovm
	}
	

	go{
		addCode(prepareDelayLine());
		if (delayMask != -1) addCode(getUpdateAddress());
		addCode(doUpdate());
		if (delayMask != -1) addCode(prepareFIR());
		addCode(doFIR());
	}		


	execTime { 
		return 32;
        }
}

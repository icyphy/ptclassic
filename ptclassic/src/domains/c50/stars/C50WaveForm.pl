defstar {
	name { WaveForm }
	domain { C50 }
	desc {
A value of values is repeated at the ouput with period period, zero-padding
or tuncating to period if necessary.  Setting period to 0 (default) ouputs
the value once.  The default value is 0.1 0.2 0.3 0.4.
        }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q5 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 signal sources library }
        explanation {
You can get periodic signals with any period, and can halt a simulation
at the end of the given waveform.  The following table summarizes the
capabilities:
.TS
center, box;
l l l l.
haltAtEnd	periodic	period	operation
_
NO	YES	0	Period is the length of the waveform.
NO	YES	N > 0	Period is N.
NO	NO	anything	Output the waveform once, then zeros.
.TE
The first line of the table gives the default settings.
.PP
This star may be used to read a file by simply setting "value" to
something of the form "< filename".  The file will be read completely
and its contents stored in an array.  The size of the array is currently
limited to 20,000 samples.
.IE "waveform from file"
.IE "reading from a file"
.IE "halting a simulation"
        }

        ccinclude { "Scheduler.h" }

        output {
		name { output }
		type { fix }
	}
	state {
		name { value }
		type { fixarray }
                desc { One period of the output waveform. }
		default { "0.1 0.2 0.3 0.4" }
                attributes { A_CIRC|A_NONCONSTANT|A_UMEM}
	}
        state {
	        name { haltAtEnd }
	        type { int }
	        default { "NO" }
	        desc { Halt the run at the end of the given data. }
	}
        state {
                name { periodic }
	        type { int }
	        default { "YES" }
	        desc { Output is periodic if "YES" (nonzero). }
        }
	state {
		name { period }
		type { int }
                desc {If greater than zero, gives the period of the waveform}
		default { 0 }
	}
        state  {
                name { X }
                type { int }
                default { 0 }
                desc { internal }
               attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
        state  {
                name { firstVal }
                type { fix }
                default { 0.1 }
                desc { first value of value }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
        state {
		name { valueLen }
		type { int }
		desc { number of values in value. }
		default { 4 }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
        state {
                name { dataCirc }
                type { int }
                desc { pointer }
                default { 0 }
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_NOINIT }
        }

        codeblock (org) {
	.ds   	#$addr(output)		; initialization of DC block
        }
        codeblock (dc) {
        .q15	#$val(firstVal)
        }
        codeblock (orgp) {
        .text
        }
        codeblock (impulse) {
	mar	*,AR6				; impulse
	lar 	AR7,#$addr(output)		;Address output		=> AR7
        lar     AR6,#$addr(value)		;Address value		=> AR6
	lacc   	*,0,AR7				;Accu = value
        sacl   	*,0,AR6				;ouput = value
	zap					;clear P-Reg and Accu
        sacl    *				;value = 0
        }
        codeblock (aperiodic) {
        mar	*,AR2				; aperiodic value
	lar	AR2,$addr(dataCirc)		;Address dataCirc	=> AR2
        zap   					;clear P-Reg. and Accu
	bldd	*,#$addr(output)		;output = dataCirc
        sacl	*+				;dataCirc = 0     incr AR2
        sar 	*,#$addr(dataCirc)		;store new dataCircle
        }
        codeblock (periodperiodicSequence) {
	mar 	*,AR2				; period periodic value
        lar	AR2,#$addr(dataCirc)		;Address dataCirc	=> AR2
        nop					;waste cycles
	lacc	*				;waste cycles
        bldd    *+,#$addr(output)		;output = dataCirc incr AR2  
        sar	*,#$addr(dataCirc)		;store new dataCircle
        }    
        codeblock (zeroPaddedSequence) {
	mar	*,AR2				; Zero padded value
	lmmr	ARCR,#$val(valueLen)		;valueLen => ARCR
	lar     AR2,#$addr(dataCirc)		;Address dataCirc	=> AR2
	lar	AR2,*
        cmpr	2				;AR2 > ARCR
        bcnd	$label(l29),NTC			;if no jump to label(l29)
	lar	AR1,#0.0			;AR1 = zero
	lmmr	ARCR,#$val(X)			;X  => ARCR
        smmr   	AR2,#$addr(output)		;output zero as sample value
        cmpr    2				;AR2 > ARCR
    	adrk	#1				;AR2 = AR2 +1
        bcnd    $label(l30),NTC			;if no jump to label(l130)
	smmr	AR1,#$addr(dataCirc)		;dataCirc = 0
        b     	$label(l28)			;jump label(l28)
$label(l29)			
	mar	*,AR2
	adrk	#$addr(value)			;AR2 = AR2 + address from value
        bldd	*+,#$addr(output)		;output =         incr AR2 
	sbrk	#$addr(value)			;AR2 = AR2 - address from value
$label(l30)
        smmr	AR2,#$addr(dataCirc) 		;store incremented counter 
$label(l28)
        }

        codeblock(makeblock) {
	.ds	$addr(dataCirc)		; output sample count
       	.int	1,0
	.text
	}            

        codeblock(initDataCirc) {
        .ds     $ref(dataCirc)		; initialization of dataCirc
        .word   $addr(value)
        .text
        }

	setup {
		firstVal = value[0];
		valueLen = value.size();
		if (!(int (haltAtEnd)) &&int (periodic) && period == 0)
			period = valueLen;
		if (!(int (haltAtEnd)) &&!(int (periodic)))
			period = 0;
		if (period == 1) 
			output.setAttributes(P_NOINIT);
		if ((period > valueLen) || (period == 0))
			value.resize(int (valueLen));
		else
			value.resize(int (period));
	}
	initCode {
		if (period == 1) {
			//special case, reproduce DC star.
				addCode(org);
			for (int i = 0; i < output.bufSize(); i++)
				addCode(dc);
			addCode(orgp);
		}
		if (period != 1) {
			if ((period == 0 && valueLen > 1) ||
			   (period != 0 && period <= valueLen)) {
				addCode(initDataCirc);
			} else {
				if (period > valueLen)
					addCode(makeblock);
			}
		}
	}
	go {
                if (int(haltAtEnd))
                        Scheduler::requestHalt();
		if (period == 1)
			return;
		//special case, output stored at compile time.
			if (period == 0) {
			if (valueLen == 1)
				addCode(impulse);
			//output impulse.
				else {
				X = valueLen - 1;
				addCode(aperiodic);
				//output general aperiodic value.
			}
		}
		if (period <= valueLen && period != 1 && period != 0) {
			//output periodic value-- use first period values.
				X = period - 1;
			addCode(periodperiodicSequence);
		}
		if (period > valueLen && period != 1 && period != 0) {
			//output periodic value-- zero padded.
				X = period - 1;
			addCode(zeroPaddedSequence);
		}
	}

	execTime {
		if (int (period) == 0)
			if (int (valueLen) == 1)
				return 7;
			else
				return 8;
		if (int (period) == 1)
			return 0;
		if (int (period) <= int (valueLen))
			return 8;
		return 14;

	}
}



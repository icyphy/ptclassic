defstar {
	name { WaveForm }
	domain { C50 }
	desc {
A value of values is repeated at the output with period period, zero-padding
or truncating to period if necessary.  Setting period to 0 (default) output
the value once.  The default value is 0.1 0.2 0.3 0.4.
        }
	version { $Id$ }
	author { Luis Gutierrez, A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
You can get periodic signals with any period, and can halt a simulation
at the end of the given waveform.  The following table summarizes the
capabilities:
<p>
<pre>
  haltAtEnd  periodic   period     operation
--------------------------------------------------------------------------------
   NO        YES        0          Period is the length of the waveform.
   NO        YES        N > 0      Period is N.
   NO        NO         anything   Output the waveform once, then zeros.
   YES       anything   anything   Halt the run at end of given data.
</pre>
<p>
The first line of the table gives the default settings.
<p>
This star may be used to read a file by simply setting "value" to
something of the form "&lt; filename".  The file will be read completely
and its contents stored in an array.  The size of the array is currently
limited to 20,000 samples.
<a name="waveform from file"></a>
<a name="reading from a file"></a>
<a name="halting a simulation"></a>
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

	protected {
		int X,len;
	}

        codeblock (org) {
	.ds   	$addr(output)		; initialization of DC block
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
	lacl   	*,AR7				;Accu = value
        sacl   	*,AR6				;output = value
        sach    *,0				;value = 0
        }

	codeblock( initCircBuff,""){
	lacc	#$addr(value)
	samm	cbsr1
	add	#@X
	samm	cber1
	lacl	#10				; setup circ buff with
	samm	cbcr				; ar2 as circ buff pointer
	}

	codeblock (restoreCircBuff){
	zap
	samm	cbcr			; disable circ buff
	}
	 

        codeblock (periodicSequence) {
	lmmr	AR2,#$addr(dataCirc)		;Address dataCirc	=> AR2
	mar 	*,AR2				; period periodic value
        bldd    *+,#$addr(output)		;output = dataCirc incr AR2  
        smmr	ar2,#$addr(dataCirc)		;store new dataCircle
        }    

        codeblock (zeroPaddedSequence,"") {
	mar	*,ar2
	lmmr	ar2,#$addr(dataCirc)
	lacc	#$addr(value,@valueLen)
	samm	arcr
	cmpr	1	
	lar	ar3,#$addr(output)
	xc	1,TC
	lacc	*,16
	mar	*+,ar3
	sach	*,0
	smmr	ar2,#$addr(dataCirc)
	}

        codeblock(makeblock) {
	.ds	$addr(dataCirc)		; output sample count
       	.int	1,0
	.text
	}            

        codeblock(initDataCirc) {
        .ds     $addr(dataCirc)		; initialization of dataCirc
        .word   $addr(value)
        .text
        }

	setup {
		firstVal = value[0];
		valueLen = value.size();
		if (!(int(haltAtEnd)) && int(periodic) && int(period) == 0)
			period = valueLen;
		if (!(int(haltAtEnd)) && !(int(periodic)))
			period = 0;
		if (int(period) == 1) 
			output.setAttributes(P_NOINIT);
		if ((int(period) > valueLen) || (int(period) == 0))
			value.resize(int(valueLen));
		else
			value.resize(int(period));
	}
	initCode {
		if (int(period) == 1) {
			// special case, reproduce Const star.
			addCode(org);
			for (int i = 0; i < output.bufSize(); i++) addCode(dc);
			addCode(orgp);
		}
		else {
			addCode(initDataCirc);

		}
	}
	go {
                if (int(haltAtEnd)) Scheduler::requestHalt();

		// special case, output stored at compile time.
		if (int(period) == 1) return;

		if (int(period) == 0) {
			if (int(valueLen) == 1)		// output impulse
				addCode(impulse);
			else {		// output general aperiodic value.
				X = int(valueLen) - 1;
				addCode(zeroPaddedSequence());
			}
		}

		if (int(period) <= int(valueLen) && int(period) != 1 &&
		    int(period) != 0) {
			//output periodic value-- use first period values.
			X = int(period) - 1;
			addCode(initCircBuff());
			addCode(periodicSequence);
			addCode(restoreCircBuff);
		}

		if (int(period) > int(valueLen) && int(period) != 1 &&
		    int(period) != 0) {
			//output periodic value-- zero padded.
			X = int(period) - 1;
			addCode(initCircBuff());
			addCode(zeroPaddedSequence());
			addCode(restoreCircBuff);
		}
	}

	execTime {
		if (int(period) == 0) {
			if (int(valueLen) == 1) return 6;
			else return 6;
		}
		if (int(period) == 1) return 0;
		if (int(period) <= int(valueLen)) return 12;
		return 19;
	}
}








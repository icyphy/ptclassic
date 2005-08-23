defstar {
	name { WaveForm }
	domain { CG56 }
	desc {
A value of values is repeated at the output with period period, zero-padding
or truncating to period if necessary.
Setting period to 0 (default) outputs the value once.
The default value is 0.1 0.2 0.3 0.4.
        }
	version { @(#)CG56WaveForm.pl	1.19  03/29/97 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
You can get periodic signals with any period, and can halt a simulation
at the end of the given waveform.
The following table summarizes the capabilities:

<TABLE BORDER>
<TR> <TH> haltAtEnd <TH> periodic <TH> period <TH> operation </TR>
<TR> <TD> NO </TD> <TD> YES </TD> <TD> 0 </TD> <TD> Period is the length of the waveform. </TD> </TR>
<TR> <TD> NO </TD> <TD> YES </TD> <TD> N &gt; 0 </TD> <TD> Period is N. </TD> </TR>
<TR> <TD> NO </TD> <TD> NO </TD> <TD> anything </TD> <TD> Output the waveform once, then zeros. </TD> </TR>
</TABLE>

<!-- haltAtEnd    periodic    period    operation -->
<!-- -------------------------------------------------- -->
<!--    NO          YES         0      Period is the length of the waveform. -->
<!--    NO          YES      N &lt; 0     Period is N. -->
<!--    NO          NO       anything  Output the waveform once, then zeros. -->

<p>
The first line of the table gives the default settings.
<p>
This star may be used to read a file by simply setting "value" to
something of the form "&lt; filename".
The file will be read completely and its contents stored in an array.
The size of the array is currently limited to 20,000 samples.
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
                attributes { A_CIRC|A_NONCONSTANT|A_YMEM}
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
                attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM|A_NOINIT }
        }

        codeblock (org) {
; initialization of DC block
        org     $ref(output)
        }
        codeblock (dc) {
        dc      $val(firstVal)
        }
        codeblock (orgp) {
        org     p:
        }
        codeblock (impulse) {
; impulse
        clr     b       $ref(value),a
        move    a,$ref(output)
        move    b,$ref(value)
        }
        codeblock (aperiodic) {
; aperiodic value
        move    $ref(dataCirc),r0
        move    #$val(X),m0
        clr     b       y:(r0),a
        move    b,y:(r0)+
        move    a,$ref(output)
        move    r0,$ref(dataCirc)
        move    m7,m0
        }
        codeblock (periodperiodicSequence) {
; period periodic value
        move    $ref(dataCirc),r0
        move    #$val(X),m0
        nop
        move    y:(r0)+,a
        move    a,$ref(output)
        move    r0,$ref(dataCirc)
        move    m7,m0
        }    
        codeblock (zeroPaddedSequence) {
; Zero padded value
        move    #>$val(valueLen),x1
        move    $ref(dataCirc),b
        move    #$addr(value),n0
        cmp     x1,b    b,r0
        jlt     $label(l29)
; #samples output > valueLen -- output zero as sample value
        clr     a       #>$val(X),x1
        move    a,$ref(output)
        cmp     x1,b    (r0)+
        jlt     $label(l30)
; end of period -- reset counter
        move    a,$ref(dataCirc)
        jmp     $label(l28)
$label(l29)
; output value from data value as sample value
        move    y:(r0+n0),a
        move    (r0)+
        move    a,$ref(output)
$label(l30)
; store incremented counter
        move    r0,$ref(dataCirc)  
$label(l28)
        }

        codeblock(makeblock) {
; output sample count
	org	$ref(dataCirc)
       	bsc	1,0
	org	p:
	}            

        codeblock(initDataCirc) {
; initialization of dataCirc
        org     $ref(dataCirc)
        dc      $addr(value)
        org     p:
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
		if ((int(period) > int(valueLen)) || (int(period) == 0))
			value.resize(int(valueLen));
		else
			value.resize(int(period));
	}
	initCode {
		if (int(period) == 1) {
			// special case, reproduce Const star.
			addCode(org);
			for (int i = 0; i < output.bufSize(); i++)
				addCode(dc);
			addCode(orgp);
		}
		else {
			if ((int(period) == 0 && int(valueLen) > 1) ||
			   (int(period) != 0 && int(period) <= int(valueLen))) {
				addCode(initDataCirc);
			}
			else if (int(period) > int(valueLen)) {
				addCode(makeblock);
			}
		}
	}
	go {
                if (int(haltAtEnd)) Scheduler::requestHalt();

		// special case, output stored at compile time.
		if (int(period) == 1) return;

		if (int(period) == 0) {
			if (int(valueLen) == 1)		// output impulse.
				addCode(impulse);
			else {		// output general aperiodic value.
				X = int(valueLen) - 1;
				addCode(aperiodic);
			}
		}

		if (int(period) <= int(valueLen) && int(period) != 1 &&
		    int(period) != 0) {
			// output periodic value-- use first period values.
			X = int(period) - 1;
			addCode(periodperiodicSequence);
		}
		if (int(period) > int(valueLen) && int(period) != 1 &&
		    int(period) != 0) {
			// output periodic value-- zero padded.
			X = int(period) - 1;
			addCode(zeroPaddedSequence);
		}
	}

	execTime {
		if (int(period) == 0) {
			if (int (valueLen) == 1) return 3;
			else return 9;
		}
		if (int(period) == 1) return 0;
		if (int(period) <= int(valueLen)) return 9;
		return 19;

	}
}

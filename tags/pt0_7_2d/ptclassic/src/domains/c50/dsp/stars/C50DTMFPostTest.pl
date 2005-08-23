defstar{
	name { DTMFPostTest }
	domain { C50 }
	version { @(#)C50DTMFPostTest.pl	1.7	06 Oct 1996 }
	desc { 
Returns whether or not a valid dual-tone modulated-frequency has
been correctly detected based on the last three detection results.
        }
	author { Luis J. Gutierrez, based on CG56 version }
	htmldoc {
The assumption is that the 100 msec DTMF interval has been split into
roughly four parts.  This star looks at the last three detection results,
which are represented as integers.  A new digit has been detected
if two consecutive detected digits are the same followed by a third
detected digit that is different.
<p>
This test is useful for two reasons.
First, it filters redundant hits so that only one is reported.
Second, it improves robustness against noisy DTMF signals and speech input.
<h3>References</h3>
<p>[1]  
Pat Mock, "Add DTMF Generation and Decoding to DSP-uP Designs,"
Electronic Data News, March 21, 1985.  Reprinted in
<i>Digital Signal Processing Applications with the TMS320 Family</i>,
Texas Instruments, 1986.
	}
	copyright { 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
	location { C50 dsp library }
	input {
		name { input }
		type { int }
	}
	input {
		name { valid }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	state {
		name { last }
		type { int }
		default { "-1" }
		desc { internal }
		attributes {A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_CONSEC}
	}
	state{
		name { secondToLast }
		type { int }
		default { "-1" }
		desc { internal }
		attributes {A_NONCONSTANT|A_NONSETTABLE|A_UMEM|A_CONSEC}
	}
	state { 
		name { initialLastInput }
		type { int }
		default { "-1" }
		desc{
The value to assign to last and secondToLast internal states, which hold
the previous two valid inputs.  For the purposes of DTMF detection, it
is set to an integer that does not represent a DTMF digit.
        	}
		attributes{ A_SETTABLE|A_UMEM }
	}
	codeblock(postTest){
	lar	ar1,#$addr(last)		; ar1->[last,2nd2last]
	lar	ar2,#$addr(valid)		; ar2->valid
	mar	*,ar2				; arp = 2
	lacc	*,16,ar1			; acc = "valid"; arp = 0
	lmmr	arcr,#$addr(input)		; arcr = "input"
	xc	2,EQ				; is acc == "valid" == 0?
	lmmr	arcr,#$addr(initialLastInput)
	lar	ar3,*+,ar3			; ar3 = "last"; ar1-> 2nd2last; arp = 3
	cmpr	3				; test if last <> input
	mar	*,ar1				; arp = 0
	xc	1,TC				; if last <> input then "valid" = 0
	zap	
	lar	ar4,*,ar4			; ar4 = 2nd2last; arp = 4
	cmpr	0				; test if 2nd2last == input
	mar	*,ar1				; arp = 0
	xc	1,TC				;if 2nd2last == input then "valid" = 0
	zap
	sar	ar3,*,ar5			; last == ar3 -> 2nd2last; arp = ar5
	smmr	arcr,#$addr(last)		; last = arcr == input
	lar	ar5,#$addr(output)		; ar5 -> output
	sach	*				; move acc to output		
	}

	setup {
		secondToLast = int(initialLastInput);
		last = int(initialLastInput);
	}

 	go{
		addCode(postTest);
	}

	// Return an execution estimate in pairs of cycles
	exectime{ return 11; }
}

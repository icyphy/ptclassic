defstar {
	name { AOut }
	domain { C50 }
	desc { An output star for the DSK320 analog output port }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 io library }
	explanation {
.PP
This star is an interrupt driven D/A star for the DSK320 board.
The star outputs one sample each time it fires.
The output voltage at the RCA connector is scaled to maximum 3V.
.PP
If the star is repeated in a schedule (for example, if it is
connected to a star that consumes more than one sample each time
it fires), interrupt-based code will be generated.
The interrupt buffer will be the minimum required size if the
parameter \fIinterruptBufferSize\fP is "default".
If this parameter is a number, it will be used for the length
(in words) of the interrupt buffer.
.PP

	}
        seealso { AIn }
	input {
		name {input}
		type {FIX}
	}

        state {
		name { interruptBufferSize }
                type { int }
                desc {
	    size of interrupt buffer, presently does not support repetition.
	        }
             	default { 4 }
 	}
	state {
		name { bufLen }
		type { int }
  		desc { internal }
		default { 4 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	state {
		name { starInBufPtr }
		type { fix }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_BMEM|A_NOINIT }
	}
	state {
		name { inIntBuffer }
		type { intarray }
  		desc { internal }
		default { "0" }
 		attributes {A_CIRC|A_NONSETTABLE|A_NONCONSTANT|A_BMEM|A_NOINIT}
	}
	state {
		name { inIntBufferStart }
		type { int }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_BMEM|A_NOINIT }
	}

        codeblock(interruptInit) {
	splk	#12h,IMR			; Turn on receive interrupt
	splk    #20h,TCR
	splk    #01h,PRD			; 10 MHz Master Clock for AIC 
	mar     *,AR0
	lacc    #0008h				; non continuous mode
	sacl    SPC				; FSX as input
	lacc    #00c8h				; 16 bits words
	sacl    SPC
	lacc    #080h				; pulse AIC reset by setting it
	sach    DXR				; low
	sacl    GREG
	lar     AR0,#0ffffh
	rpt     #10000				; and taking it high after 
	 lacc   *,0,AR0				; 10000 cycles (.5ms at 50 ns)
	sach    GREG
	;
	setc    SXM				; initialize TA and RA register
	lacc    #03060h				; for 288kHz SCF clock freq.
	call    AIC_2ND				; (=> cut freq. 8kHz)
	;
	lacc    #0244ah				; initialize TB and TA register
	call    AIC_2ND				; for sampling freq. 16 kHz
	;					  set control register:
	lacc    #0067h				; bandpass / synchr. transmit
	call    AIC_2ND				; gain: input 3V = full scale
	;
        }
        codeblock(interruptOut) {
	mar	*,AR6
	lar	AR6,#$addr(input)		;Address input		=>AR6
	splk	#12h,IMR			;enable transmit interrupt
	splk	#$val(bufLen),BRCR		;number of loops into BRCR
	rptb	$label(lpOut)			;loop to label(lpOut)
	 lacc	*				;Accu = input
	 add    #08000h,0			;Accu = Accu -1.0
	 and    #0fffch				;clear bits 0 and 1
	 samm	DXR				;Accu => analog output
	 idle					;wait for interrupt
$label(lpOut)
	splk	#02h,IMR			;disable transmit interrupt
        }
	setup {
		bufLen = interruptBufferSize;
		inIntBuffer.resize(bufLen);
	}
	initCode {
		addCode(interruptInit);
	}
	go {
		addCode(interruptOut);
	}
	execTime {
		return 3+6*int(bufLen);
	}
}    





















































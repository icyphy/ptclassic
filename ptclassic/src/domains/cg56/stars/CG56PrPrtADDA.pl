defstar {
	name { PrPrtADDA }
	domain { CG56 }
	desc { An input/output star for the Ariel ProPort }
	version { @(#)CG56PrPrtADDA.pl	1.16 03/29/97 }
	acknowledge { Gabriel version by Phil Lapsley }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<p>
This star is an interrupt driven A/D and D/A star
for the Ariel Proport board.
The star inputs one sample from each of the two input channels
and outputs one sample to each of the two output channels each
time it fires.
<p>
If the star is repeated in a schedule (for example, if it is
connected to a star that consumes more than one sample each time
it fires), interrupt-based code will be generated.
If the star is not repeated, it will generate code
that polls the Proport and busy waits if samples are not available.
Interrupt-based code can be forced by setting the string
parameter <i>forceInterrupts</i> to "yes".
The interrupt buffer will be the minimum required size if the
parameter <i>interruptBufferSize</i> is "default=4".
If this parameter is a number, it will be used for the length
(in words) of the interrupt buffer.
<p>
In the event of a real-time violation, execution will abort
and one of the following error codes will be left in register y0:
<p><b>123062</b>  
An interrupt occurred and the input buffer was full.
<p><b>123063</b>  
An interrupt occurred and the output buffer was empty.
	}
        seealso { PrPrtAD, PrPrtDA }
	input {
		name {input1}
		type {FIX}
	}
	input {
		name {input2}
		type {FIX}
	}
	output {
		name {output1}
		type {FIX}
	}
	output {
		name {output2}
		type {FIX}
	}
	state {
		name { forceInterrupts }
		type { string }
		desc { If YES use interrupts, otherwise use polling. }
		default { "No" }		
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
		name { abortOnRealTimeError }
		type { string }
		desc { if yes, abort on rreal time error }
		default { "yes" }
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
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT }
	}
	state {
		name { starOutBufPtr }
		type { fix }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
	}
	state {
		name { saveReg }
		type { fixarray }
  		desc { internal }
		default { "0" }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
	}
	state {
		name { inIntBuffer }
		type { intarray }
  		desc { internal }
		default { "0" }
 		attributes
			{A_CIRC|A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
	}
	state {
		name { inIntBufferStart }
		type { int }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT }
	}
	state {
		name { outIntBuffer }
		type { intarray }
  		desc { internal }
		default { "0" }
 		attributes {A_CIRC|A_NONSETTABLE|A_NONSETTABLE|A_XMEM|A_NOINIT}
	}
	state {
		name { outIntBufferStart }
		type { int }
  		desc { internal }
		default { 0 }
 		attributes {A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
	} 	
        codeblock(pollingInit) {
; Polling version
; Configure SSI Port: Control Register A
; 16 bit word length, 2 frame network mode
        movep   #$$4100,x:m_cra
; Configure SSI port: control register B
; rx/tx enabled, network mode, no interrupts
        movep   #$$3A00,x:m_crb
; Configure Port C pins 8-5 as SSI pins
        bset    #8,x:m_pcc
        bset    #7,x:m_pcc
        bset    #6,x:m_pcc
        bset    #5,x:m_pcc
        }

        codeblock(abortyes) {
; Polling version
; Check for receiver overrun error
        jclr    #m_roe,x:m_sr,$label(cont)
        move    #$$123062,y0
        jmp     ERROR
$label(cont)
        }

        codeblock(polling) {
; Process Channel One
$label(loopOne)
        jclr    #m_rdf,x:m_sr,$label(loopOne)
        movep   x:m_rx,x0
        move    x0,$ref(output1)
        move    $ref(input1),x0
        movep   x0,x:m_tx
; Process Channel Two
$label(loopTwo)
        jclr    #m_rdf,x:m_sr,$label(loopTwo)
        movep   x:m_rx,x0
        move    x0,$ref(output2)
        move    $ref(input2),x0
        movep   x0,x:m_tx
        }

        codeblock(interruptInit) {
$starSymbol(proport)_savereg   equ     $addr(saveReg)
$starSymbol(proport)_buflen    equ     $val(bufLen)
$starSymbol(proport)_insptr    equ     $addr(starInBufPtr)
$starSymbol(proport)_iniptr    equ     $addr(inIntBufferStart)
$starSymbol(proport)_outsptr   equ     $addr(starOutBufPtr)
$starSymbol(proport)_outiptr   equ     $addr(outIntBufferStart)

; Interrupt handler's next-word-to-write pointer

        org     $ref(inIntBufferStart)
        dc      $addr(inIntBuffer)
        org     p:
    
; Interrupt handler's next-word-to-read pointer
        org     $ref(outIntBufferStart)
        dc      $addr(outIntBuffer)
        org     p:
; Empty input buffer by setting bit 0 in each word
        move    x:$starSymbol(proport)_iniptr,r0
        move    r0,x:$starSymbol(proport)_insptr
        move    #$starSymbol(proport)_buflen-1,m0
        rep     #$val(bufLen)
        bset    #0,x:(r0)+
        move    #-1,m0
; Fill output buffer by clearing bit 0 in each word
        move    x:$starSymbol(proport)_outiptr,r0
        move    r0,x:$starSymbol(proport)_outsptr
        move    #$starSymbol(proport)_buflen-1,m0
        rep     #$val(bufLen)
        bclr    #0,x:(r0)+
        move    #-1,m0
        }
        codeblock(interrupt) {
; SSI receive data interrupt vector
        org     p:i_ssird
        jsr     $starSymbol(proport)_intr
        }
        codeblock(interrupt1) {
; SSI receive data w/ exception interrupt vector
        org     p:i_ssirde
        jsr     $starSymbol(proport)_intr
        }
        codeblock(interruptCont) {
; set SSI interrupt priority = 2
        bset    #m_ssl0,x:m_ipr
        bset    #m_ssl1,x:m_ipr
; Configure SSI port: control register A
; 16 bit word length, 2 frame network mode
        movep   #$$4100,x:m_cra
; Configure SSI port: control register B
; rx/tx enabled, rx int enabled, network mode
        movep   #$$BA00,x:m_crb
; Configure Port C pins 8-5 as SSI pins
        bset    #8,x:m_pcc
        bset    #7,x:m_pcc
        bset    #6,x:m_pcc
        bset    #5,x:m_pcc
        }    
        codeblock(interruptIn) {
; Input first sample from interrupt buffer
        move    x:$starSymbol(proport)_insptr,r0  ; Get pointer to input position
        move    #$starSymbol(proport)_buflen-1,m0 ; and set modulus
$label(fillOne)
        jset    #0,x:(r0),$label(fillOne) ; Wait for position to have data
        move    x:(r0),y0               ; Get sample from buffer
        bset    #0,x:(r0)+              ; Mark position as empty
        move    y0,$ref(output1)
; Input second sample from interrupt buffer
$label(fillTwo)
        jset    #0,x:(r0),$label(fillTwo) ; Wait for position to have data
        move    x:(r0),y0               ; Get sample from buffer
        bset    #0,x:(r0)+              ; Mark position as empty
        move    y0,$ref(output2)
        move    r0,x:$starSymbol(proport)_insptr ; Save pointer to next position
        move    #-1,m0                  ; Restore modulus
; Output first sample to interrupt buffer
        move    x:$starSymbol(proport)_outsptr,r0  ; Get pointer to output position
        move    #$starSymbol(proport)_buflen-1,m0  ; and set modulus
        move    $ref(input1),y0
$label(emptyOne)
        jclr    #0,x:(r0),$label(emptyOne) ; Wait for position to be empty
        move    y0,x:(r0)               ; Put data there
        bclr    #0,x:(r0)+              ; Mark position as full
; Output second sample to interrupt buffer
        move    $ref(input2),y0
$label(emptyTwo)
        jclr    #0,x:(r0),$label(emptyTwo) ; Wait for position to be empty
        move    y0,x:(r0)               ; Put data there
        bclr    #0,x:(r0)+              ; Mark position as full
        move    r0,x:$starSymbol(proport)_outsptr ; Save pointer to next position
        move    #-1,m0                  ; Restore modulus
        }
        codeblock(interruptTerminate) {
; Input/output interrupt handler for $fullname()
$starSymbol(proport)_intr
        move    y0,x:$starSymbol(proport)_savereg+0      ; Save y0, r0, m0
        move    r0,x:$starSymbol(proport)_savereg+1
        move    m0,x:$starSymbol(proport)_savereg+2
        move    #$starSymbol(proport)_buflen-1,m0
; Input sample and update buffers
        move    x:$starSymbol(proport)_iniptr,r0 ;r0 points to where to save sample
        nop
        jset    #0,x:(r0),$label(used)  ; Error if it's already used
        move    #$$123062,y0
        jmp     ERROR
$label(used)
        move    x:m_rx,y0
        move    y0,x:(r0)
        bclr    #0,x:(r0)+              ; Mark location as used
        move    r0,x:$starSymbol(proport)_iniptr    ; Save updated pointer
; Output sample and update buffers
        move    x:$starSymbol(proport)_outiptr,r0  ; r0 points to sample to output
        nop
        jclr    #0,x:(r0),$label(empty)  ; Error if it's empty
        move    #$$123063,y0
        jmp     ERROR
$label(empty)
        move    x:(r0),y0
        bset    #0,x:(r0)+              ; Mark location as empty
        move    r0,x:$starSymbol(proport)_outiptr    ; Save updated pointer
        move    y0,x:m_tx
        move    x:$starSymbol(proport)_savereg+0,y0      ; Restore y0, r0, m0
        move    x:$starSymbol(proport)_savereg+1,r0
        move    x:$starSymbol(proport)_savereg+2,m0
        rti
        }        
	setup {
		bufLen = interruptBufferSize;
		saveReg.resize(3);
		inIntBuffer.resize(bufLen);
		outIntBuffer.resize(bufLen);
	}
	initCode {
		const char     *f = forceInterrupts;
		if (f[0] == 'n' || f[0] == 'N')
			addCode(pollingInit);
		else {
			addCode(interruptInit);
			genInterruptCode(interrupt);
			genInterruptCode(interrupt1);
			addCode(interruptCont);
		}
	}

	go {
		const char     *p = forceInterrupts;
		const char     *q = abortOnRealTimeError;
		if (p[0] == 'n' || p[0] == 'N') {
			//polling
				if (q[0] == 'y' || q[0] == 'Y')
				addCode(abortyes);
			addCode(polling);
		} else {
			addCode(interruptIn);
		}
	}
	execTime {
		const char     *a = forceInterrupts;
		if (a[0] == 'n' || a[0] == 'N')
			return 21;
		else
			return 73;
	}
	wrapup {
		const char     *i = forceInterrupts;
		if (i[0] == 'y' || i[0] == 'Y') {
			addCode(interruptTerminate);
		}
	}
}    

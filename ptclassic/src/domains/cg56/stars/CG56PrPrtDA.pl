defstar {
	name { ProPortDA }
	domain { CG56 }
	desc { An input/output star for the Ariel ProPort }
	version { $Id$ }
	acknowledge { Gabriel version by Phil Lapsley }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 io library }
	explanation {
.PP
This star is an interrupt driven D/A star for the Ariel Proport board.
The star takes its two inputs and outputs them to the two
channels on the Proport board each time it fires.
.PP
If the star is repeated in a schedule (for example, if it is
connected to a star that consumes more than one sample each time
it fires), interrupt-based code will be generated.
If the star is not repeated, it will generate code
that polls the Proport and busy waits if samples are not available.
Interrupt-based code can be forced by setting the string
parameter \fIforceInterrupts\fP to "yes".
The interrupt buffer will be the minimum required size if the
parameter \fIinterruptBufferSize\fP is "default=4".
If this parameter is a number, it will be used for the length
(in words) of the interrupt buffer.
.PP
In the event of a real-time violation, execution will abort
and the following error code will be left in register y0:
.ip "\fB123061\fP"
An interrupt occurred and the output buffer was empty.
	}
        seealso { ProPortAD, ProPortADDA }
	input {
		name {input1}
		type {FIX}
	}
	input {
		name {input2}
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
		name { outIntBuffer }
		type { intarray }
  		desc { internal }
		default { "0" }
		attributes
			{A_CIRC|A_NONSETTABLE|A_NONSETTABLE|A_XMEM|A_NOINIT}
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
        move    $ref(input1),x0
        movep   x0,x:m_tx
; Process Channel Two
$label(loopTwo)
        jclr    #m_rdf,x:m_sr,$label(loopTwo)
        movep   x:m_rx,x0
        move    $ref(input2),x0
        movep   x0,x:m_tx
        }

        codeblock(interruptInit) {
$starSymbol(proportda)_savereg   equ     $addr(saveReg)
$starSymbol(proportda)_buflen    equ     $val(bufLen)
$starSymbol(proportda)_outsptr   equ     $addr(starOutBufPtr)
$starSymbol(proportda)_outiptr   equ     $addr(outIntBufferStart)

; Interrupt handler's next-word-to-write pointer
        org     $ref(outIntBufferStart)
        dc      $addr(outIntBuffer)
        org     p:
; Fill output buffer by clearing bit 0 in each word
        move    x:$starSymbol(proportda)_outiptr,r0
        move    r0,x:$starSymbol(proportda)_outsptr
        move    #$starSymbol(proportda)_buflen-1,m0
        rep     #$val(bufLen)
        bclr    #0,x:(r0)+
        move    #-1,m0
        }
        codeblock(interrupt) {
; SSI receive data interrupt vector
        org     p:i_ssird
        jsr     $starSymbol(proportda)_intr
        }
        codeblock(interrupt1) {
; SSI receive data w/ exception interrupt vector
        org     p:i_ssirde
        jsr     $starSymbol(proportda)_intr
        }
        codeblock(interruptCont) {
; set SSI interrupt priority = 2
        bset    #m_ssl0,x:m_ipr
        bset    #m_ssl1,x:m_ipr
; Configure SSI Port: Control Register A
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
; Output first sample to interrupt buffer
        move    x:$starSymbol(proportda)_outsptr,r0  ; Get pointer to output position
        move    #$starSymbol(proportda)_buflen-1,m0  ; and set modulus
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
        move    r0,x:$starSymbol(proportda)_outsptr ; Save pointer to next position
        move    #-1,m0                  ; Restore modulus
        }
        codeblock(interruptTerminate) {
; Input/output interrupt handler for $fullname()
$starSymbol(proportda)_intr
        move    y0,x:$starSymbol(proportda)_savereg+0      ; Save y0, r0, m0
        move    r0,x:$starSymbol(proportda)_savereg+1
        move    m0,x:$starSymbol(proportda)_savereg+2
        move    #$starSymbol(proportda)_buflen-1,m0
; Output sample and update buffers
        move    x:$starSymbol(proportda)_outiptr,r0  ; r0 points to sample to output
        nop
        jclr    #0,x:(r0),$label(empty)  ; Error if it's empty
        move    #$$123061,y0
        jmp     ERROR
$label(empty)
        move    x:(r0),y0
        bset    #0,x:(r0)+              ; Mark location as empty
        move    r0,x:$starSymbol(proportda)_outiptr    ; Save updated pointer
        move    y0,x:m_tx
        move    x:$starSymbol(proportda)_savereg+0,y0      ; Restore y0, r0, m0
        move    x:$starSymbol(proportda)_savereg+1,r0
        move    x:$starSymbol(proportda)_savereg+2,m0
        rti
        }        
	start {
		bufLen = interruptBufferSize;
		saveReg.resize(3);
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
			return 19;
		else
			return 67;
	}
	wrapup {
		const char     *i = forceInterrupts;
		if (i[0] == 'y' || i[0] == 'Y') {
			addCode(interruptTerminate);
		}
	}
}    

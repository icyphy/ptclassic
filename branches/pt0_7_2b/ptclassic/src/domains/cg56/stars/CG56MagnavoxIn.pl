defstar {
	name { MagnavoxIn }
	domain { CG56 }
	desc { 
DSP56000 -  A combined input/output star for the Magnavox CD player.
	}
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
DSP56000 -  A combined input/output star for the Magnavox CD player.
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
 		attributes {A_CIRC|A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
	}
	state {
		name { inIntBufferStart }
		type { int }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT }
	}
        codeblock(pollingInit) {
; Polling version
; Configure SSI Port: Control Register A
        bset    #m_wl1,x:m_cra          ; SSI word length = 16
; Configure SSI Port: Control Register B
        bset    #m_fsl,x:m_crb          ; SSI TX & RX frame-sync pulse width = \
1 bit
        bset    #m_syn,x:m_crb          ; SSI TX & RX Synchronous
        bset    #m_ste,x:m_crb          ; SSI Transmit enable
        bset    #m_sre,x:m_crb          ; SSI Receive enable
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
        move    #$$123052,y0
        jmp     ERROR
$label(cont)
        }

        codeblock(polling) {
; Process Channel One
$label(loopOne)
        jclr    #m_rdf,x:m_sr,$label(loopOne)
        movep   x:m_rx,x0
        move    x0,$ref(output1)
        movep   x0,x:m_tx
; Process Channel Two
$label(loopTwo)
        jclr    #m_rdf,x:m_sr,$label(loopTwo)
        movep   x:m_rx,x0
        move    x0,$ref(output2)
        movep   x0,x:m_tx
        }

        codeblock(interruptInit) {
$starSymbol(mag)_savereg   equ     $addr(saveReg)
$starSymbol(mag)_buflen    equ     $val(bufLen)
$starSymbol(mag)_insptr    equ     $addr(starInBufPtr)
$starSymbol(mag)_iniptr    equ     $addr(inIntBufferStart)

; Interrupt handler's next-word-to-write pointer

        org     $ref(inIntBufferStart)
        dc      $addr(inIntBuffer)
        org     p:
    
; Empty input buffer by setting bit 0 in each word
        move    x:$starSymbol(mag)_iniptr,r0
        move    r0,x:$starSymbol(mag)_insptr
        move    #$starSymbol(mag)_buflen-1,m0
        rep     #$val(bufLen)
        bset    #0,x:(r0)+
        move    #-1,m0
        }
        codeblock(interrupt) {
; SSI receive data interrupt vector
        org     p:i_ssird
        jsr     $starSymbol(mag)_intr
        }
        codeblock(interruptCont) {
; set SSI interrupt priority = 2
        bset    #m_ssl0,x:m_ipr
        bset    #m_ssl1,x:m_ipr
; Configure SSI Port: Control Register A
        bset    #m_wl1,x:m_cra          ; SSI word length = 16
; Configure SSI Port: Control Register B
        bset    #m_fsl,x:m_crb     ; SSI TX & RX frame-sync pulse width = 1 bit
        bset    #m_syn,x:m_crb          ; SSI TX & RX Synchronous
        bset    #m_ste,x:m_crb          ; SSI Transmit enable
        bset    #m_sre,x:m_crb          ; SSI Receive enable
; Configure Port C pins 8-5 as SSI pins
        bset    #8,x:m_pcc
        bset    #7,x:m_pcc
        bset    #6,x:m_pcc
        bset    #5,x:m_pcc
; Enable interrupts
        bset    #m_srie,x:m_crb         ; SSI Receive Interrupt enable
        }    
        codeblock(interruptIn) {
; Input first sample from interrupt buffer
        move    x:$starSymbol(mag)_insptr,r0  ; Get pointer to input position
        move    #$starSymbol(mag)_buflen-1,m0 ; and set modulus
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
        move    r0,x:$starSymbol(mag)_insptr ; Save pointer to next position
        move    #-1,m0                  ; Restore modulus
        }
        codeblock(interruptTerminate) {
; Input/output interrupt handler for 56magnavox1
$starSymbol(mag)_intr
        move    y0,x:$starSymbol(mag)_savereg+0      ; Save y0, r0, m0
        move    r0,x:$starSymbol(mag)_savereg+1
        move    m0,x:$starSymbol(mag)_savereg+2
        move    #$starSymbol(mag)_buflen-1,m0
; Input sample and update buffers
        move    x:$starSymbol(mag)_iniptr,r0 ;r0 points to where to save sample
        nop
        jset    #0,x:(r0),$label(used)  ; Error if it's already used
        move    #$$123050,y0
        jmp     ERROR
$label(used)
        move    x:m_rx,y0
        move    y0,x:(r0)
        bclr    #0,x:(r0)+              ; Mark location as used
        move    r0,x:$starSymbol(mag)_iniptr    ; Save updated pointer
        move    y0,x:m_tx
        move    x:$starSymbol(mag)_savereg+0,y0      ; Restore y0, r0, m0
        move    x:$starSymbol(mag)_savereg+1,r0
        move    x:$starSymbol(mag)_savereg+2,m0
        rti
        }        
        start {
        bufLen=interruptBufferSize;
        saveReg.resize(3);
	inIntBuffer.resize(bufLen);
	}
        initCode {
        const char* f = forceInterrupts;
	     if (f[0]=='n' || f[0]=='N')
	          gencode(pollingInit);
             else {
	          gencode(interruptInit);
		  genInterruptCode(interrupt);
		  gencode(interruptCont);
             }
	}	   

	go {
        const char* p = forceInterrupts;
        const char* q = abortOnRealTimeError;
    	      if (p[0]=='n' || p[0]=='N') {
                  // polling
                  if (q[0]=='y' || q[0]=='Y') 
	               gencode(abortyes);
                  gencode(polling);
	      }
              else {
	          gencode(interruptIn);
              }
        }
	execTime {
        const char* a = forceInterrupts;               
    	      if (a[0]=='n' || a[0]=='N') 
                  return 19;
              else
	          return 67;
	}    
        wrapup {
        const char* i = forceInterrupts;               
    	      if (i[0]=='y' || i[0]=='Y') {
	          gencode(interruptTerminate);
              }
        }
}    
defstar {
	name { Magnavox }
	domain { CG56 }
	desc { 
DSP56000 -  A combined input/output star for the Magnavox CD player.
	}
	version { $Id$ }
	author { Chih-Tsung Huang, J. Pino, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
DSP56000 -  A combined input/output star for the Magnavox CD player.
	}
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
                type { string }
                desc { size of interrupt buffer }
             	default { "default" }
 	}
	state {
		name { abortOnRealTimeError }
		type { string }
		desc { if yes, abort on rreal time error }
		default { "yes" }
	}
	state {
		name { starInBufptr }
		type { fix }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
	}
	state {
		name { starOutBufptr }
		type { fix }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
	}
	state {
		name { saveReg }
		type { fix }
  		desc { internal }
		default { 0 }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
	}
	state {
		name { inIntBuffer }
		type { fix }
  		desc { internal }
		default { 0 }
 		attributes { A_CIRC|A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
	}
	state {
		name { outIntBuffer }
		type { fix }
  		desc { internal }
		default { 0 }
 		attributes { A_CIRC|A_NONSETTABLE|A_NONSETTABLE|A_XMEM }
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

        initCode {
        const char* f = forceInterrupts;
	     if (f[0]=='n' || f[0]=='N')
	          gencode(pollingInit);
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
        }
	execTime {
                  return 21;
	}
        wrapup {
        const char* i = forceInterrupts;               
  //  	      if (i[0]=='y' || i[0]=='Y') 
                  // interrupts
        }
}

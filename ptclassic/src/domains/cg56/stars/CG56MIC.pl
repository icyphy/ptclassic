defstar {
	name { MIC }
	domain { CG56 }
        desc { An input star for the Ariel digital microphone }
        version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
.PP
This star produces left and right channel sample outputs from
the Ariel digital microphone.
.PP
The parameter \fIsampleRate\fP can be one of
88200, 44100, 22050, 11025, or 5512.
Of course, since both left and right channel samples are produced
each time the star fires, the effective sample rate is really twice the
value indicated.
.PP
The star will generate polling-based code if it is
not repeated in a schedule; if it is repeated, interrupt-based code
will be generated.
Interrupt code can be forced by setting the \fIforceInterrupt\fP
parameter to "yes".
Additionally, the interrupt buffer size can be set to a user-defined number
of words by supplying a numeric parameter for
\fIinterruptBufferSize\fP.
This is useful when talking to a device with suitable average throughput but
high latency, e.g., a Sun workstation.
The default buffering size can be obtained by making this parameter
the string "default=4".
.PP
Realtime violations will be ignored if the parameter
\fIabortOnRealtimeError\fP is "no".
If it is "yes",
they will cause the universe to abort execution with hex error code
123041 in register y0.
.PP
The digital microphone can be used with an ADS system or the Dolby board.
Variable sample rates are not supported on the Dolby board at the present
time, and the sample rate will be 44100 regardless of the \fIsampleRate\fP
parameter.
.SH BUGS
The parameter \fIabortOnRealtimeError\fP is currently ignored if
interrupt-based code is generated, and the universe will abort on a realtime
violation in this mode regardless of the value of the parameter.
	}
        
        output {
		name { output1 }
		type { fix }
	}
        output {
		name { output2}
		type { fix }
	}
        state  {
                name { sampleRate }
                type { int }
                default { 44100 }
                desc { specifies the sample rate }
        }
	state {
		name { forceInterrupts }
		type { string }
		desc { force interrupts }
		default { "no" }
	}
	state {
		name { interruptBufferSize }
		type { string }
		desc { size of interrupt buffer }
		default { "default" }
	}
        state {
                name { abortOnRealtimeError }
                type { string }
                desc { internal }
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
                name { starInBufptr }
	        type { fix }
	        desc { internal }
                default { 0 }
	        attributes { A_NONCONSTANT|A_NONSETTABLE|A_XMEM|A_NOINIT }
        }
        state {
                name { saveReg }
                type { fixarray }
                desc { internal }
                default { "0" }
                attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT }
        }        
        state {
                name { intBuffer }
                type { intarray }
                desc { internal }
                default { "0" }
                attributes {A_CIRC|A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
        }        
        state {
                name { intBufferStart }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT }
        }        
	    
        codeblock (pollingInit) {
; SSI Control Register A
; 16 bits per word, two words per frame
        movep   #$$4100,x:m_cra
; SSI Control Register B
; Enable receiver in network mode
        movep   #$$2A00,x:m_crb
; Configure PC pins 7, 6, 5 (SRD, SCK, SC2) as SSI pins
        bset    #7,x:m_pcc
        bset    #6,x:m_pcc
        bset    #5,x:m_pcc
; Configure PC pins 2, 1, and 0 as general purpose I/O
        bclr    #2,x:m_pcc
        bclr    #1,x:m_pcc
        bclr    #0,x:m_pcc
; Configure PC pins 2, 1, and 0 as outputs for sample rate control
        bset    #2,x:m_pcddr
        bset    #1,x:m_pcddr
        bset    #0,x:m_pcddr
        }
        codeblock(eight) {
; Set sample rate to 88200 samples/sec
        bclr    #2,x:m_pcd
        bclr    #1,x:m_pcd
        bclr    #0,x:m_pcd
        }    
        codeblock(four) {
; Set sample rate to 44100 samples/sec
        bset    #2,x:m_pcd
        bset    #1,x:m_pcd
        bset    #0,x:m_pcd
        }
        codeblock(two) {
; Set sample rate to 22050 samples/sec
        bclr    #2,x:m_pcd
        bset    #1,x:m_pcd
        bclr    #0,x:m_pcd
        }    
        codeblock(one) {
; Set sample rate to 11025 samples/sec
        bclr    #2,x:m_pcd
        bclr    #1,x:m_pcd
        bset    #0,x:m_pcd
        }    
        codeblock(five) {
; Set sample rate to 5512 samples/sec
        bclr    #2,x:m_pcd
        bset    #1,x:m_pcd
        bset    #0,x:m_pcd
        }    

        codeblock(abortyes) {
; Polling version
; Check for receiver overrun error
        jclr    #m_roe,x:m_sr,$label(cont)
        move    #$$123041,y0
        jmp     ERROR
$label(cont)
        }
        codeblock(polling) {
; Process channel 1
$label(one)
        jclr    #m_rdf,x:m_sr,$label(one)
        movep   x:m_rx,x0
        move    x0,$ref(output1)
; Process channel 2
$label(two)
        jclr    #m_rdf,x:m_sr,$label(two)
        movep   x:m_rx,x0
        move    x0,$ref(output2)
        }    
        codeblock(interruptEquate) {
; $starSymbol(mic) interrupt circular buffer equates
$starSymbol(mic)_savereg   equ     $addr(saveReg)
$starSymbol(mic)_buflen    equ     $val(bufLen)
$starSymbol(mic)_insptr    equ     $addr(starInBufptr)
$starSymbol(mic)_iniptr    equ     $addr(intBufferStart)

; Interrupt handler's next-word-to-write pointer
        org     $ref(intBufferStart)
        dc      $addr(intBuffer)
        org     p:

; Empty input buffer by setting bit 0 in each word
        move    x:$starSymbol(mic)_iniptr,r0
        move    r0,x:$starSymbol(mic)_insptr
        move    #$starSymbol(mic)_buflen-1,m0
        rep     #$val(bufLen)
        bset    #0,x:(r0)+
        move    #-1,m0
        }
        codeblock(interruptInit) {
        org     p:i_ssird
        jsr     $starSymbol(mic)_intr
        }    
        codeblock(interruptCont) {
; Set SSI interrupt to priority 2
        bset    #12,x:m_ipr
        bset    #13,x:m_ipr
; SSI Control Register A
; 16 bits per word, two words per frame
        movep   #$$4100,x:m_cra
; SSI Control Register B
; Enable receiver and rx interrupts in network mode
        movep   #$$AA00,x:m_crb
; Configure PC pins 7, 6, 5 (SRD, SCK, SC2) as SSI pins
        bset    #7,x:m_pcc
        bset    #6,x:m_pcc
        bset    #5,x:m_pcc
; Configure PC pins 2, 1, and 0 as outputs for sample rate control
        bset    #2,x:m_pcddr
        bset    #1,x:m_pcddr
        bset    #0,x:m_pcddr
        }
        codeblock(interruptIn) {
; Input first sample from interrupt buffer
        move    x:$starSymbol(mic)_insptr,r0       ; Get pointer to input position
        move    #$starSymbol(mic)_buflen-1,m0      ; and set modulus
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
        move    r0,x:$starSymbol(mic)_insptr       ; Save pointer to next position
        move    #-1,m0                  ; Restore modulus
        }    
        codeblock(interruptTerminate) {
; Input interrupt handler for 56mic1
$starSymbol(mic)_intr
        move    y0,x:$starSymbol(mic)_savereg+0    ; Save y0, r0, m0
        move    r0,x:$starSymbol(mic)_savereg+1
        move    m0,x:$starSymbol(mic)_savereg+2
        move    #$starSymbol(mic)_buflen-1,m0
; Input sample and update buffers
        move    x:$starSymbol(mic)_iniptr,r0       ; r0 points to where to save sample
        nop
        jset    #0,x:(r0),$label(empty) ; Error if it's already used
        move    #$$123041,y0
        jmp     ERROR
$label(empty)
        movep   x:m_rx,y0
        move    y0,x:(r0)
        bclr    #0,x:(r0)+              ; Mark location as used
        move    r0,x:$starSymbol(mic)_iniptr       ; Save updated pointer
        move    x:$starSymbol(mic)_savereg+0,y0    ; Restore y0, r0, m0
        move    x:$starSymbol(mic)_savereg+1,r0
        move    x:$starSymbol(mic)_savereg+2,m0
        rti
        }    
        start {
        saveReg.resize(3);
        intBuffer.resize(bufLen);
        const char* a=forceInterrupts;
	const char* b=abortOnRealtimeError;
                switch(a[0]) {
	        case 'Y': case 'y': case 'n': case 'N':
                      break;
                default:
        	      Error::abortRun(*this, "forceInterrupts must be yes or no.");
                }
                switch(b[0]) {
	        case 'Y': case 'y': case 'n': case 'N':
                      break;
                default:	    
        	      Error::abortRun(*this, "abortRealtimeError must be yes or no.");
                }
                if (sampleRate!=88200 && sampleRate!=44100 && sampleRate!=22050 && sampleRate!=11025 && sampleRate!=5512)
        	      Error::abortRun(*this, "sampleRate must be 88200, 44100, 22050, 11025, or 5512.");                		      
         }    
 
        initCode { 
        const char* p = forceInterrupts;
             if (p[0]=='n' || p[0]=='N') {
                      gencode(pollingInit);            // polling
                      if (sampleRate==88200) gencode(eight);
		      if (sampleRate==44100) gencode(four);
		      if (sampleRate==22050) gencode(two);
		      if (sampleRate==11025) gencode(one);
		      if (sampleRate==5512)  gencode(five);
             }
             else {
                      gencode(interruptEquate);
                      genInterruptCode(interruptInit);           
                      gencode(interruptCont);
                      if (sampleRate==88200) gencode(eight);
		      if (sampleRate==44100) gencode(four);
		      if (sampleRate==22050) gencode(two);
		      if (sampleRate==11025) gencode(one);
		      if (sampleRate==5512)  gencode(five);
             }
        }

        go { 
        const char* q = abortOnRealtimeError;
        const char* z = forceInterrupts;
             if (z[0]=='n' || z[0]=='N') {
                 if (q[0]=='y' || q[0]=='Y')
                      gencode(abortyes);
                 gencode(polling);
             }
             else {
	         gencode(interruptIn);
             }		 
        }
	execTime { 
        const char* y = forceInterrupts;
             if (y[0]=='n' || y[0]=='N')
	           return 15;
	     else
	           return 61;
        }
        wrapup {
        const char* i = forceInterrupts;
              if (i[0]=='y' || i[0]=='Y') {
                  gencode(interruptTerminate);
              }
        }

}

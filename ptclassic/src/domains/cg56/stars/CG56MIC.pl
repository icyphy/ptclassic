defstar {
	name { MIC }
	domain { CG56 }
        desc { An input star for the Ariel digital microphone }
        version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
        explanation {
DSP56000 - An input star for the Ariel digital microphone
           Parameter 'sample_rate' can be one of 88200, 44100,
           22050, 11025, or 5512.
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
                name { starInBufptr }
	        type { fix }
	        desc { internal }
                default { 0 }
	        attributes { A_NONCONSTANT|A_NONSETTABLE|A_XMEM }
        }
        state {
                name { saveReg }
                type { fix }
                desc { internal }
                default { 0 }
                attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
        }        
        state {
                name { intBuffer }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_CIRC|A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
        }        
        state {
                name { intBufferPtr }
                type { int }
                desc { internal }
                default { 0 }
                attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM }
        }        
	    
        codeblock (pollingInit) {
; SSI Control Register A
; 16 bits per word, two words per frame
        movep   #$4100,x:m_cra
; SSI Control Register B
; Enable receiver in network mode
        movep   #$2A00,x:m_crb
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

        start {
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
                if (sampleRate!=88200 || sampleRate!=44100 || sampleRate!=22050 || sampleRate!=11025 || sampleRate!=5512)
        	      Error::abortRun(*this, "SampleRate must be 88200, 44100,22050, 11025, or 5512.");                		      
 
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
             else  
                      return;
                    
        }

        go { 
        const char* q = abortOnRealtimeError;
        const char* z = forceInterrupts;
             if (z[0]=='n' || z[0]=='N') {
                 if (q[0]=='y' || q[0]=='Y')
                      gencode(abortyes);
                 gencode(polling);
             }

        }
	execTime { 
        const char* y = forceInterrupts;
             if (y[0]=='n' || y[0]=='N')
	           return 15;
	     else
	           return 61;
        }
}

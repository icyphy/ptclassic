defstar {
    name { SSI }
    domain { CG56 }
    desc { A generic input/output star the 560001 SSI port. }
    version { $Id$ }
    author { Kennard White, Chih-Tsung Huang (ported from Gabriel) }
    acknowledge { Gabriel author Phil Lapsley. }
    copyright { 1992 The Regents of the University of California }
    location { CG56 library }
    explanation {
.PP
This star is a generic star to provide input/ouput for the 560001's
SSI port (Synchronous Serial Interface).  It can implement both a
synchronous, in-line interface and an interupt-driven interface.
The star inputs one sample from each of the two input channels
and outputs one sample to each of the two output channels each
time it fires.  The samples from the star's two input ports
are transmited out the SSI port,
and samples received from the SSI port are available on the star's
two outputs.
The star could be modified to support different
sample rates on input and output.
.PP
This star is commonly used as a base class for the Ariel Proport A/D and D/A
converter and the modified Magnavox CD player.
.PP
If the star is repeated in a schedule (for example, if it is
connected to a star that consumes more than one sample each time
it fires), interrupt-based code will be generated.
If the star is not repeated, it will generate code
that polls the SSI and busy waits if samples are not available.
Interrupt-based code can be forced by setting the buffer size positive.
The interrupt buffer holds at least \fIqueueSize\fP samples; the length
of the queue will be adjusted upward according to the number of schedule
repetitions of the star.
.PP
If a real-time violation occurs and the parameter
\fIabortOnRealtimeError\fP is TRUE, execution will abort
and one of the following error codes will be left in register y0:
.IP "\fB123062\fP"
An interrupt occurred and the receive buffer was full.
.IP "\fB123063\fP"
An interrupt occurred and the transmit buffer was empty.
.SH INTERUPTS: QUEUES
When using interupt based code, the SSI port generates interupts
which are handled by an procedure.  This procedure transmits samples
out of a xmit queue, and writes received samples to a recv queue.
The syncronous star code transfers data between these queues
and the star portholes.  The data words in each buffer are called
"slots", and at any given moment in time, a is either full (has
a valid sample) or is empty.
There are two approaches to managing the
memory and syncronization of these queues; these are described below.
.SH INTERUPTS: DUAL-BUFFER QUEUEING
Two buffers are maintained:
a recv buffer and an xmit buffer.  The data words in a buffer are called
"slots".  If a slot has valid data in it, bit #0 is cleared, otherwise
it is set.  The interrupt handler reads from the SSI placing
the word in the recv buffer and clears the bit; it then takes a word
from the xmit buffer, sets bit #0 in the slot, and writes it to the SSI.
.PP
The only reason to use dual-buffering is to support different recv&xmit
rates.  In this case two independent buffers are required with
independent pointers for each stream.  Note also
that the SDF parameters of the star must coorespond to the relative
recv&xmit rates, or buffer overflow/underrun will occur.
.PP
This star was originally written to use dual buffers, but has never
(and still doesn't) support differing recv&xmit rates.  The code
preserves this style for future use, but in general it should not be used.
.SH SYMETRIC QUEUEING
Symetric buffer queueing may be used only when the recv&xmit samples rates
are the same.  In this case, we can always access the recv&xmit samples
in pairs.  We align the two differs symetrically in X: and Y: memory.
This has the advantage that a single pointer can be used to walk through
the queues instead of two paraell pointers.  Semaphoring of slots
filled/empty status is easier: only one slot of the (recv/xmit)
pair need be marked.  If bit #0 is set in the recv sample, the slot
is empty and next slot is full.
.PP
To implement syncronized recv&xmit on the 56001, we used symetric memory,
taking advantage of the fact that we have independent X: and Y: memory.
This is not an intrinsic requirement: instead, we could have interleaved
the recv&xmit buffers together with an xmit slot following each recv slot.
The only difficulty would be holding off semaphoring the recv slot in
the star code: this would require using an extra register to preserve
the address until after the xmit had been taken care of.
    }
    seealso { ProPortAD, ProPortDA, MagnavoxIn, MagnavoxOut }
    hinclude { <stream.h> }
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
	    name { hardware }
	    type { string }
	    desc { Name of physical hardware attached to SSI. }
	    default { "PROPORT" }		
    }
    state {
	    name { symmetricBuffers }
	    type { int }
	    desc { If TRUE use symetric queuing, else use dual buffering. }
	    default { "YES" }		
    }
    state {
	    name { queueSize }
	    type { int }
	    desc { Length of interupt queue (0==>polling). }
	    default { 4 }
    }
    state {
	    name { abortOnRealTimeError }
	    type { int }
	    desc { If TRUE, abort on real time violation. }
	    default { "YES" }
    }
    state {
	    name { cra }
	    type { int }
	    desc { Port C SSI control register A (custom hardware only). }
	    default { 0 }
    }
    state {
	    name { crb }
	    type { int }
	    desc { Port C SSI control register B (custom hardware only). }
	    default { 0 }
    }
    state {
	    name { bufLen }
	    type { int }
	    desc { internal }
	    default { 4 }
	    attributes { A_NONSETTABLE|A_NONCONSTANT }
    }
    state {
	    name { intrAbort }
	    type { int }
	    desc { Integer form of abortOnRealTimeError (for macros). }
	    attributes { A_NONSETTABLE|A_NONCONSTANT }
	    default { 1 }
    }
    state {
	    name { dualbufFlag }
	    type { int }
	    desc { state version of !doSymmetric }
	    default { 0 }
	    attributes { A_NONSETTABLE|A_NONCONSTANT }
    }
    state {
	    name { buffer }
	    type { fixarray }
	    desc { internal }
	    default { "0" }
	    attributes {A_CIRC|A_NONSETTABLE|A_NONCONSTANT|A_SYMMETRIC|A_RAM|A_NOINIT}
    }
    state {
	    name { recvStarPtr }
	    type { int }
	    desc { internal }
	    default { 0 }
	    attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT }
    }
    state {
	    name { xmitStarPtr }
	    type { int }
	    desc { internal }
	    default { 0 }
	    attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
    }
    state {
	    name { recvIntrPtr }
	    type { int }
	    desc { internal }
	    default { 0 }
	    attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT }
    }
    state {
	    name { xmitIntrPtr }
	    type { int }
	    desc { internal }
	    default { 0 }
	    attributes {A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
    } 	
    state {
	    name { saveReg }
	    type { fixarray }
	    desc { internal }
	    default { "0" }
	    attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
    }
    state {
	    name { savePrevOut }
	    type { INT }
	    desc { Boolean flag indicating to save previous output values. }
	    default { "0" }
	    attributes { A_NONSETTABLE|A_NONCONSTANT}
    }
    state {
	    name { prevOut1 }
	    type { FIX }
	    desc { Copy of previous output#1 sample. }
	    default { "0" }
	    attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
    }
    state {
	    name { prevOut2 }
	    type { FIX }
	    desc { Copy of previous output#2 sample. }
	    default { "0" }
	    attributes { A_NONSETTABLE|A_NONCONSTANT|A_XMEM|A_NOINIT}
    }
    protected {
	int	doIntr;
	int	doErrAbort;
	int	doSymmetric;
	int	doRecv1;
	int	doRecv2;
	int	doXmit1;
	int	doXmit2;
    }

    codeblock(ssiInit) {
	movep	#$val(cra),x:m_cra
	movep	#$val(crb),x:m_crb
; Configure Port C pins 8-5 as SSI pins
        bset    #8,x:m_pcc		; STD
        bset    #7,x:m_pcc		; SRD
        bset    #6,x:m_pcc		; SCK (bit clock)
        bset    #5,x:m_pcc		; SC2 (frame clock)
; Configure Port C pins 2-0 as raw data pins (normally SCI)
        bclr    #2,x:m_pcc
        bclr    #1,x:m_pcc
        bclr    #0,x:m_pcc
        bset    #2,x:m_pcddr		; as outputs
        bset    #1,x:m_pcddr
        bset    #0,x:m_pcddr
    }
    codeblock(enableInterupts) {
        bset    #m_ssl0,x:m_ipr		; set SSI IPL 2
        bset    #m_ssl1,x:m_ipr
        bset    #m_srie,x:m_crb         ; enable SSI rx interupts
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
        jclr    #m_rdf,x:m_sr,*
        movep   x:m_rx,x0
        move    x0,$ref(output1)
        move    $ref(input1),x0
        movep   x0,x:m_tx
; Process Channel Two
        jclr    #m_rdf,x:m_sr,*
        movep   x:m_rx,x0
        move    x0,$ref(output2)
        move    $ref(input2),x0
        movep   x0,x:m_tx
    }

    codeblock(interruptInit) {
$starSymbol(ssi)_savereg	equ	$addr(saveReg)
$starSymbol(ssi)_buflen		equ	$val(bufLen)
$starSymbol(ssi)_buffer		equ	$addr(buffer)
$starSymbol(ssi)_recv_sptr	equ	$addr(recvStarPtr)
$starSymbol(ssi)_recv_iptr	equ	$addr(recvIntrPtr)
$starSymbol(ssi)_xmit_sptr	equ	$addr(xmitStarPtr)
$starSymbol(ssi)_xmit_iptr	equ	$addr(xmitIntrPtr)
$starSymbol(ssi)_dualbuf	equ	$val(dualbufFlag)

; Initialize all the pointers to the right place.
; Note that recv&xmit bufs are at the same add but recv in x: and xmit in y:
        org     $ref(recvStarPtr)
        dc      $starSymbol(ssi)_buffer
        org     $ref(recvIntrPtr)
        dc      $starSymbol(ssi)_buffer
        org     $ref(xmitStarPtr)
        dc      $starSymbol(ssi)_buffer
        org     $ref(xmitIntrPtr)
        dc      $starSymbol(ssi)_buffer
        org     p:
    
        move    #$starSymbol(ssi)_buffer,r0
	.LOOP	#$starSymbol(ssi)_buflen
          bset    #0,x:(r0)	; empty recv buf by setting  bit 0
          bclr    #0,y:(r0)+	; fill  xmit buf by clearing bit 0
	.ENDL
    }
    codeblock(interruptRecvDataVec) {
; SSI receive data interrupt vector
        org     p:i_ssird
        jsr     $starSymbol(ssi)_intr
    }
    codeblock(interruptRecvExcptVec) {
; SSI receive data w/ exception interrupt vector
; XXX: this is wrong!!!
        org     p:i_ssirde
        jsr     $starSymbol(ssi)_intr
    }

    ////////////////////////////////////////////////////////////////////////
    //
    //			Interupt: syncronous star code
    //
    ////////////////////////////////////////////////////////////////////////

    codeblock(intrLoadM0) {
	move    #$starSymbol(ssi)_buflen-1,m0
    }
    codeblock(intrRestoreM0) {
	move    m7,m0
    }
    codeblock(intrLoadRecvPtr) {
	move    x:$starSymbol(ssi)_recv_sptr,r0
	nop
    }
    codeblock(intrSaveRecvPtr) {
	move    r0,x:$starSymbol(ssi)_recv_sptr
    }
    codeblock(intrGetRecvY0) {
        jset    #0,x:(r0),*		; Wait for slot to have data
        move    x:(r0),y0               ; Get sample from buffer
	IF	$val(dualbufFlag)
          bset    #0,x:(r0)+              ; Mark slot as empty
	ENDIF
    }
    codeblock(intrGetRecv1) {
        move    y0,$ref(output1)
	IF	$val(savePrevOut)
	  move	  y0,$ref(prevOut1)
	ENDIF
    }
    codeblock(intrGetRecv2) {
        move    y0,$ref(output2)
	IF	$val(savePrevOut)
	  move	  y0,$ref(prevOut2)
	ENDIF
    }

    codeblock(intrLoadXmitPtr) {
	move    x:$starSymbol(ssi)_xmit_sptr,r0
    }
    codeblock(intrSaveXmitPtr) {
	move    r0,x:$starSymbol(ssi)_xmit_sptr
    }
    codeblock(intrPutXmit1) {
	move    $ref(input1),y0
    }
    codeblock(intrPutXmit2) {
	move    $ref(input2),y0
    }
    codeblock(intrPutXmitY0) {
	IF $val(dualbufFlag)
          jclr    #0,y:(r0),*		; Wait for slot to be empty
	ENDIF
        move    y0,y:(r0)               ; Put data there
	IF $val(dualbufFlag)
          bclr    #0,y:(r0)+              ; Mark slot as full
	ELSE
          bset    #0,x:(r0)+              ; Mark slot as empty
	ENDIF
    }
    method {
	name {go_intr_port} arglist {"(const char *port)"}
	code {
	    /*IF*/ if ( strcmp(port,"xmit1")==0 ) {
		gencode(intrPutXmit1);
		gencode(intrPutXmitY0);
	    } else if ( strcmp(port,"xmit2")==0 ) {
		gencode(intrPutXmit2);
		gencode(intrPutXmitY0);
	    } else if ( strcmp(port,"recv1")==0 ) {
		gencode(intrGetRecvY0);
		gencode(intrGetRecv1);
	    } else if ( strcmp(port,"recv2")==0 ) {
		gencode(intrGetRecvY0);
		gencode(intrGetRecv2);
	    } else {
		Error::abortRun(*this,": invalid port");
	    }
	}
    }

    method {
	name { go_intr }
	code {
	    gencode(intrLoadM0);
	    if ( doSymmetric ) {
		gencode(intrLoadRecvPtr);
		if ( doRecv1 || doXmit1 ) {
		    go_intr_port("recv1");
		    go_intr_port("xmit1");
		}
		if ( doRecv2 || doXmit2 ) {
		    go_intr_port("recv2");
		    go_intr_port("xmit2");
		}
		gencode(intrSaveRecvPtr);
	    } else {
		if ( doRecv1 || doRecv2 ) {
		    gencode(intrLoadRecvPtr);
		    if ( doRecv1 )		go_intr_port("recv1");
		    if ( doRecv2 )		go_intr_port("recv2");
		    gencode(intrSaveRecvPtr);
		}
		if ( doXmit1 || doXmit2 ) {
		    gencode(intrLoadXmitPtr);
		    if ( doXmit1 )		go_intr_port("xmit1");
		    if ( doXmit2 )		go_intr_port("xmit2");
		    gencode(intrSaveXmitPtr);
		}
	    }
	    gencode(intrRestoreM0);
	}
    }


    ////////////////////////////////////////////////////////////////////////
    //
    //			Interupt: intr handler code
    //
    ////////////////////////////////////////////////////////////////////////

    codeblock(ihdlPreserveRegs) {
; Interrupt handler for $fullname()
$starSymbol(ssi)_intr
        move    y0,x:$starSymbol(ssi)_savereg+0      ; Save y0, r0, m0
        move    r0,x:$starSymbol(ssi)_savereg+1
        move    m0,x:$starSymbol(ssi)_savereg+2
        move    #$starSymbol(ssi)_buflen-1,m0
    }
    codeblock(ihdlRestoreRegs) {
        move    x:$starSymbol(ssi)_savereg+0,y0      ; Restore y0, r0, m0
        move    x:$starSymbol(ssi)_savereg+1,r0
        move    x:$starSymbol(ssi)_savereg+2,m0
	rti
    }        

    codeblock(ihdlSingleBuffer) {
        move    x:$starSymbol(ssi)_recv_iptr,r0	; recv pointer
        move    x:m_rx,y0
        jset    #0,x:(r0),$label(doRecv)	; make sure recv slot empty
	IF	$val(intrAbort)
          move	#$$123064,y0			; its full...abort
          jmp	ERROR
	ELSE
	  ; just drop recv sample in y0
	  move	y:-(r0),y0		; go back two (stereo): prev tx sample
	  move	y:-(r0),y0
	  move	y0,x:m_tx
	  jmp	$label(done)
	ENDIF
$label(doRecv)
        move    y0,x:(r0)
        move    y:(r0),y0
        bclr    #0,x:(r0)+			; mark slot as used
        move    y0,x:m_tx
        move    r0,x:$starSymbol(ssi)_recv_iptr	; save updated pointer
$label(done)
    }

    codeblock(ihdlDualBuffer) {
        move    x:$starSymbol(ssi)_recv_iptr,r0	; recv pointer
        move    x:m_rx,y0
        jset    #0,x:(r0),$label(doRecv)	; error if slot already used
	IF	$val(intrAbort)
          move    #$$123062,y0
          jmp     ERROR
	ELSE
	  jmp	  $label(rx_done)
	ENDIF
$label(doRecv)
        move    y0,x:(r0)
        bclr    #0,x:(r0)+			; mark slot as used
        move    r0,x:$starSymbol(ssi)_recv_iptr	; save updated pointer
$label(rx_done)
        move    x:$starSymbol(ssi)_xmit_iptr,r0	; xmit pointer
        nop
        jclr    #0,y:(r0),$label(doXmit)	; error if slot empty
	IF	$val(intrAbort)
          move	#$$123063,y0
          jmp	ERROR
	ELSE
	  move	y:-(r0),y0		; go back two (stereo): prev tx sample
	  move	y:-(r0),y0
	  move	y0,x:m_tx
	  jmp	$label(tx_done)
	ENDIF
$label(doXmit)
        move    y:(r0),y0
        bset    #0,y:(r0)+              ; Mark location as empty
        move    r0,x:$starSymbol(ssi)_xmit_iptr    ; Save updated pointer
        move    y0,x:m_tx
$label(tx_done)
    }


    ////////////////////////////////////////////////////////////////////////
    //
    //			CG: normal CG methods (start,initCode,go,wrapup)
    //
    ////////////////////////////////////////////////////////////////////////

    setup {
        saveReg.resize(3);

	doSymmetric = int(symmetricBuffers);
	dualbufFlag = !doSymmetric;

	doRecv1 = doRecv2 = TRUE;
	doXmit1 = doXmit2 = TRUE;

	doIntr = reps() > 1;
	if ( int(queueSize) > 0 )
	    doIntr = TRUE;

	doErrAbort = int(abortOnRealTimeError);
	intrAbort = doErrAbort ? 1 : 0;
	
	bufLen = 0;
	if ( doIntr ) {
	    bufLen = int(queueSize);
	    if ( bufLen < reps() )
		bufLen = reps();
	    if ( (doRecv1||doXmit1) && (doRecv2||doXmit2) ) 
		bufLen = int(bufLen) * 2;
	}
	buffer.resize(bufLen);
	config_hardware();
    }
    initCode {
	    if ( ! doIntr ) {
	        gencode(ssiInit);
            } else {
		genProcCode(ihdlPreserveRegs);
		if ( doSymmetric ) {
		    genProcCode(ihdlSingleBuffer);
		} else {
		    genProcCode(ihdlDualBuffer);
		}
		genProcCode(ihdlRestoreRegs);

		gencode(interruptInit);
		genInterruptCode(interruptRecvDataVec);
		genInterruptCode(interruptRecvExcptVec);
		gencode(ssiInit);
		gencode(enableInterupts);
            }
    }	   
    go {
	if ( ! doIntr ) {
	    if ( doErrAbort )
		gencode(abortyes);
	    gencode(polling);
	} else {
	    go_intr();
	}
    }
    exectime {
	return doIntr ? 73 : 21;
    }

    method {
	name { config_hardware }
	code {
// Configure the control registers based on hardware type.
// The relevant registers and a quick summary of the bits:
//	PCC (x:m_pcc)
//	    bits 0-2: SCI (but used by proport) pin functions/enables
//	    bits 5-3: SSI aux. pin functions/enables 
//	    bits 8-6: SSI core pin functions/enables
//	PCDDR (x:m_pcddr)
//	    data direction for PCC when in paraell data-mode
//	PCD (x:m_pcd)
//	    data bits for PCC when in paraell data-mode
// 	CRA (x:m_cra)
//	    bits 7-0 (PM7-0): crystal clock to bit rate
//	    bits 12-8 (DC4-0): words per frame
//	    bits 14-13 (WL1-0): bits per word
//	    bit  15 (PSR): aux. to PM.
//	CRB (x:m_crb)
//	    bits 1-0 (OF1-): output flags
//	    bits 5-2 (SCKD,SCD2-0): pin directions;
//	    bit 6 (SHFD): shift dir
//	    bit 8-7 (fsl0,1): sync length;	bit 9 (syn): sync rec&xmit
//	    bit 10 (gck): gated clock;		bit 11 (mod): mode network
//	    bit 15-11 (rie,tie,re,te): interupt enables and function enables

	    /*IF*/ if ( strcasecmp(hardware,"custom")==0 ) {
		;
	    } else if ( strcasecmp(hardware,"proport")==0 ) {
		cra = 0x4100; // 16-bit words, 2 words/frame
		crb = 0x3A00;
		  // frame-sync=00 ==> word-len pulse (50% duty cycle)
		  // sync rx&tx, not gated, network mode, enable rx&tx
	    } else if ( strcasecmp(hardware,"digimic")==0 ) {
		// the "Ariel" Digital Microphone is a receive only
		// device.  However, it doesnt hurt
		// to send it data...it will just be dropped.
		// The baud rate can be selected similarly as to the proport,
		// but the conversion table is different.
		cra = 0x4100; // 16-bit words, 2 words/frame
		crb = 0x3A00;
		  // frame-sync=00 ==> word-len pulse (50% duty cycle)
		  // sync rx&tx, not gated, network mode, enable rx&tx
	    } else if ( strcasecmp(hardware,"magnavox")==0 ) {
		cra = 0x4000; // 16-bit words, 1 word/frame
		crb = 0x3300;
		  // frame-sync=10 ==> 1-bit pulse width
		  // sync rx&tx, not gated, normal mode, enable rx&tx
	    } else {
		Error::abortRun(*this,"Unknown hardware");
	    }

	    } // end of method code
    } // end of method
}    

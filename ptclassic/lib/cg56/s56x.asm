;
; This is header code for the S56X target of the CG56 domain
; Authors: Kennard
; Version: $Id$
;

; It is indended that this should always be used in conjuction
; with the s56x qckMon (see ~ptolemy/vendors/s56dsp/qckMon).
; That mean that the program that runs this on the dsp (typically load_s56x)
; should first boot the dsp, then load qckMon, then load this code.
; qckMon defines interupt vectors for:
;	I_RESET I_ILL I_STACK I_SWI I_TRACE
; In addition, it defines jsrs for vectors:
;	I_HSTCM10 (get) I_HSTCM11 (putX) I_HSTCM12 (putY) I_HSTCM13 (putP)
; Also, qckMon uses x:m_pbddr for various flag bits.

; Host port flow control interrupt handlers


;
; DMA request interupts.  These are hosed beyond belief.  Need to
; convert to newer (count-based) style of interupts.
;
	org	p:i_hstcm+2		; Host command 1
STARTR	bset	#0,x:m_pbddr		; Allow DSP writes to host port
	nop
	org	p:i_hstcm+4		; Host command 2
STOPR	bclr	#0,x:m_pbddr		; Disallow DSP writes to host port
	nop
	org	p:i_hstcm+6		; Host command 3
STARTW	bset	#1,x:m_pbddr		; Allow DSP reads from host port
	nop
	org	p:i_hstcm+8		; Host command 4
STOPW	bclr	#1,x:m_pbddr		; Disallow DSP reads from host port
	nop

;
; this is used by some simulator targets
;
	org	p:$1ff0
	nop
	stop

;
; qckMon reserves p-space below $90
;
	org	p:$90

ERROR
;	move	(r5)+
	movep	#$800,x:m_ipr		; turn off everything but host cmds
;	movep	#0,x:m_crb		; reset SSI
;	movep	#0,x:m_scr		; reset SCI
	andi	#$fc,mr			; enable interupts
	nop
	move	p:$8000,a0		; trigger host SIGUSR1 interupt
WAIT	jmp	*			; spin forever

;
;  This is the entry point
;
START
; Set device driver DMA ready flag
	bset	#m_hf2,x:m_hcr
	; XXX: These should go away when DMA is fixed
	bclr	#0,x:m_pbddr		; clear read DMA flow control bit
	bclr	#1,x:m_pbddr		; clear write DMA flow control bit

;
; Ptolemy generated code goes below here
;

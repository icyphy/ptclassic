;$Id$
;************************************************************************
;       EQUATES for DSP56000 Interrupt Vectors
;       Reference: DSP56000 User's Manual, Table 8.2, Interrupt Sources
;************************************************************************
;
; This program originally available on the Motorola DSP bulletin board.
; It is provided under a DISCLAMER OF WARRANTY available from
; Motorola DSP Operation, 6501 Wm. Cannon Drive W., Austin, Tx., 78735.
; 
;    edited by Michael C. Peck, Berkeley Camera Engineering 510-889-6960
; 
;	Update 22 Sep 91   Version 1.3   merged uppercase and lowercase versions
;	Update 10 Aug 91   Version 1.2   added missing interrupt names
;					NMI, ILL, HSTCMi
;	Update 24 Nov 90   Version 1.1   added memory space identifier
;
;************************************************************************

intequ  ident   1,3

I_RESET   EQU	p:$0000   ; hardware _RESET
I_STACK   EQU	p:$0002   ; stack error
I_TRACE   EQU	p:$0004   ; trace
I_SWI     EQU	p:$0006   ; SWI
I_IRQA    EQU	p:$0008   ; _IRQA
I_IRQB    EQU	p:$000A   ; _IRQB
 
I_SSIRD   EQU	p:$000C   ; SSI receive data
I_SSIRDE  EQU	p:$000E   ; SSI receive data with exception status
I_SSITD   EQU	p:$0010   ; SSI transmit data
I_SSITDE  EQU	p:$0012   ; SSI transmit data with exception status
 
I_SCIRD   EQU	p:$0014   ; SCI receive data
I_SCIRDE  EQU	p:$0016   ; SCI receive data with exception status
I_SCITD   EQU	p:$0018   ; SCI transmit data
I_SCIIL   EQU	p:$001A   ; SCI idle line
I_SCITM   EQU	p:$001C   ; SCI timer
I_NMI     EQU	p:$001E   ; Non-maskable, Reserved for Hardware Development
 
I_HSTRD   EQU	p:$0020   ; host receive data
I_HSTTD   EQU	p:$0022   ; host transmit data
I_HSTCM   EQU	p:$0024   ; host command (default)
I_HSTCM1  EQU	p:$0024   ; host command (default)
I_HSTCM2  EQU	p:$0026   ; host command 2
I_HSTCM3  EQU	p:$0028   ; host command 3
I_HSTCM4  EQU	p:$002A   ; host command 4
I_HSTCM5  EQU	p:$002C   ; host command 5
I_HSTCM6  EQU	p:$002E   ; host command 6
I_HSTCM7  EQU	p:$0030   ; host command 7
I_HSTCM8  EQU	p:$0032   ; host command 8
I_HSTCM9  EQU	p:$0034   ; host command 9
I_HSTCM10 EQU	p:$0036   ; host command 10
I_HSTCM11 EQU	p:$0038   ; host command 11
I_HSTCM12 EQU	p:$003A   ; host command 12
I_HSTCM13 EQU	p:$003C   ; host command 13
I_ILL     EQU	p:$003E   ; illegal instruction trap

i_reset   EQU	p:$0000   ; hardware _RESET
i_stack   EQU	p:$0002   ; stack error
i_trace   EQU	p:$0004   ; trace
i_swi     EQU	p:$0006   ; SWI
i_irqa    EQU	p:$0008   ; _IRQA
i_irqb    EQU	p:$000A   ; _IRQB
 
i_ssird   EQU	p:$000C   ; SSI receive data
i_ssirde  EQU	p:$000E   ; SSI receive data with exception status
i_ssitd   EQU	p:$0010   ; SSI transmit data
i_ssitde  EQU	p:$0012   ; SSI transmit data with exception status
 
i_scird   EQU	p:$0014   ; SCI receive data
i_scirde  EQU	p:$0016   ; SCI receive data with exception status
i_scitd   EQU	p:$0018   ; SCI transmit data
i_sciil   EQU	p:$001A   ; SCI idle line
i_scitm   EQU	p:$001C   ; SCI timer
i_nmi     EQU	p:$001E   ; Non-maskable, Reserved for Hardware Development
 
i_hstrd   EQU	p:$0020   ; host receive data
i_hsttd   EQU	p:$0022   ; host transmit data
i_hstcm   EQU	p:$0024   ; host command (default)
i_hstcm1  EQU	p:$0024   ; host command (default)
i_hstcm2  EQU	p:$0026   ; host command 2
i_hstcm3  EQU	p:$0028   ; host command 3
i_hstcm4  EQU	p:$002A   ; host command 4
i_hstcm5  EQU	p:$002C   ; host command 5
i_hstcm6  EQU	p:$002E   ; host command 6
i_hstcm7  EQU	p:$0030   ; host command 7
i_hstcm8  EQU	p:$0032   ; host command 8
i_hstcm9  EQU	p:$0034   ; host command 9
i_hstcm10 EQU	p:$0036   ; host command 10
i_hstcm11 EQU	p:$0038   ; host command 11
i_hstcm12 EQU	p:$003A   ; host command 12
i_hstcm13 EQU	p:$003C   ; host command 13
i_ill     EQU	p:$003E   ; illegal instruction trap

;$Id$
;
; This program originally available on the Motorola DSP bulletin board.
; It is provided under a DISCLAMER OF WARRANTY available from
; Motorola DSP Operation, 6501 Wm. Cannon Drive W., Austin, Tx., 78735.
; 
; Motorola Standard I/O Equates.
;    edited by Michael C. Peck, Berkeley Camera Engineering 510-889-6960
; 
;	Update 22 Sep 91   Version 1.4   merged upper and lower case versions
;	Update 11 Aug 91   Version 1.3   added Port C Control Register bits
;	Update 24 Nov 90   Version 1.2   added memory space identifier
;	Update 25 Aug 87   Version 1.1   fixed M_OF
;
;************************************************************************
;
;       EQUATES for DSP56000 I/O registers and ports
;
;************************************************************************

ioequ   ident   1,4

;------------------------------------------------------------------------
;
;       EQUATES for I/O Port Programming
;
;------------------------------------------------------------------------

;       Register Addresses

M_BCR   equ     x:$FFFE           ; Port A Bus Control Register
M_PBC   equ     x:$FFE0           ; Port B Control Register
M_PBDDR equ     x:$FFE2           ; Port B Data Direction Register
M_PBD   equ     x:$FFE4           ; Port B Data Register
M_PCC   equ     x:$FFE1           ; Port C Control Register
M_PCDDR equ     x:$FFE3           ; Port C Data Direction Register
M_PCD   equ     x:$FFE5           ; Port C Data Register

;       Port C Control Register Bit Flags

M_STD	equ	8
M_SRD	equ	7
M_SCK	equ	6
M_SC2	equ	5
M_SC1	equ	4
M_SC0	equ	3
M_SCLK	equ	2
M_TXD	equ	1
M_RXD	equ	0


;------------------------------------------------------------------------
;
;       EQUATES for Host Interface
;
;------------------------------------------------------------------------

;       Register Addresses

M_HCR   equ     x:$FFE8           ; Host Control Register
M_HSR   equ     x:$FFE9           ; Host Status Register
M_HRX   equ     x:$FFEB           ; Host Receive Data Register
M_HTX   equ     x:$FFEB           ; Host Transmit Data Register

;       Host Control Register Bit Flags

M_HRIE  equ     0               ; Host Receive Interrupt Enable
M_HTIE  equ     1               ; Host Transmit Interrupt Enable
M_HCIE  equ     2               ; Host Command Interrupt Enable
M_HF2   equ     3               ; Host Flag 2
M_HF3   equ     4               ; Host Flag 3

;       Host Status Register Bit Flags

M_HRDF  equ     0               ; Host Receive Data Full
M_HTDE  equ     1               ; Host Transmit Data Empty
M_HCP   equ     2               ; Host Command Pending
M_HF    equ     $18             ; Host Flag Mask
M_HF0   equ     3               ; Host Flag 0
M_HF1   equ     4               ; Host Flag 1
M_DMA   equ     7               ; DMA Status

;------------------------------------------------------------------------
;
;       EQUATES for Serial Communications Interface (SCI)
;
;------------------------------------------------------------------------

;       Register Addresses

M_SRXL  equ     x:$FFF4           ; SCI Receive Data Register (low)
M_SRXM  equ     x:$FFF5           ; SCI Receive Data Register (middle)
M_SRXH  equ     x:$FFF6           ; SCI Receive Data Register (high)
M_STXL  equ     x:$FFF4           ; SCI Transmit Data Register (low)
M_STXM  equ     x:$FFF5           ; SCI Transmit Data Register (middle)
M_STXH  equ     x:$FFF6           ; SCI Transmit Data Register (high)
M_STXA  equ     x:$FFF3           ; SCI Transmit Data Address Register
M_SCR   equ     x:$FFF0           ; SCI Control Register
M_SSR   equ     x:$FFF1           ; SCI Status Register
M_SCCR  equ     x:$FFF2           ; SCI Clock Control Register

;       SCI Control Register Bit Flags

M_WDS   equ     $3              ; Word Select Mask
M_WDS0  equ     0               ; Word Select 0
M_WDS1  equ     1               ; Word Select 1
M_WDS2  equ     2               ; Word Select 2
M_SBK   equ     4               ; Send Break
M_WAKE  equ     5               ; Wake-up Mode Select
M_RWI   equ     6               ; Receiver Wake-up Enable
M_WOMS  equ     7               ; Wired-OR Mode Select
M_RE    equ     8               ; Receiver Enable
M_TE    equ     9               ; Transmitter Enable
M_ILIE  equ     10              ; Idle Line Interrupt Enable
M_RIE   equ     11              ; Receive Interrupt Enable
M_TIE   equ     12              ; Transmit Interrupt Enable
M_TMIE  equ     13              ; Timer Interrupt Enable

;       SCI Status Register Bit Flags

M_TRNE  equ     0               ; Transmitter Empty
M_TDRE  equ     1               ; Transmit Data Register Empty
M_RDRF  equ     2               ; Receive Data Register Full
M_IDLE  equ     3               ; Idle Line
M_OR    equ     4               ; Overrun Error
M_PE    equ     5               ; Parity Error
M_FE    equ     6               ; Framing Error
M_R8    equ     7               ; Received Bit 8

;       SCI Clock Control Register Bit Flags

M_CD    equ     $FFF            ; Clock Divider Mask
M_COD   equ     12              ; Clock Out Divider
M_SCP   equ     13              ; Clock Prescaler
M_RCM   equ     14              ; Receive Clock Source
M_TCM   equ     15              ; Transmit Clock Source

;------------------------------------------------------------------------
;
;       EQUATES for Synchronous Serial Interface (SSI)
;
;------------------------------------------------------------------------

;       Register Addresses

M_RX    equ     x:$FFEF           ; Serial Receive Data Register
M_TX    equ     x:$FFEF           ; Serial Transmit Data Register
M_CRA   equ     x:$FFEC           ; SSI Control Register A
M_CRB   equ     x:$FFED           ; SSI Control Register B
M_SR    equ     x:$FFEE           ; SSI Status Register
M_TSR   equ     x:$FFEE           ; SSI Time Slot Register

;       SSI Control Register A Bit Flags

M_PM    equ     $FF             ; Prescale Modulus Select Mask
M_DC    equ     $1F00           ; Frame Rate Divider Control Mask
M_DC0	equ	8
M_DC1	equ	9
M_DC2	equ	10
M_DC3	equ	11
M_DC4	equ	12
M_WL    equ     $6000           ; Word Length Control Mask
M_WL0   equ     13              ; Word Length Control 0
M_WL1   equ     14              ; Word Length Control 1
M_PSR   equ     15              ; Prescaler Range

;       SSI Control Register B Bit Flags

M_OF    equ     $3              ; Serial Output Flag Mask
M_OF0   equ     0               ; Serial Output Flag 0
M_OF1   equ     1               ; Serial Output Flag 1
M_SCD   equ     $1C             ; Serial Control Direction Mask
M_SCD0  equ     2               ; Serial Control 0 Direction
M_SCD1  equ     3               ; Serial Control 1 Direction
M_SCD2  equ     4               ; Serial Control 2 Direction
M_SCKD  equ     5               ; Clock Source Direction
M_FSL   equ     8               ; Frame Sync Length
M_SYN   equ     9               ; Sync/Async Control
M_GCK   equ     10              ; Gated Clock Control
M_MOD   equ     11              ; Mode Select
M_STE   equ     12              ; SSI Transmit Enable
M_SRE   equ     13              ; SSI Receive Enable
M_STIE  equ     14              ; SSI Transmit Interrupt Enable
M_SRIE  equ     15              ; SSI Receive Interrupt Enable

;       SSI Status Register Bit Flags (Figure 11-43 page 11-73)

M_IF    equ     $2              ; Serial Input Flag Mask
M_IF0   equ     0               ; Serial Input Flag 0
M_IF1   equ     1               ; Serial Input Flag 1
M_TFS   equ     2               ; Transmit Frame Sync
M_RFS   equ     3               ; Receive Frame Sync
M_TUE   equ     4               ; Transmitter Underrun Error
M_ROE   equ     5               ; Receiver Overrun Error
M_TDE   equ     6               ; Transmit Data Register Empty
M_RDF   equ     7               ; Receive Data Register Full

;------------------------------------------------------------------------
;
;       EQUATES for Exception Processing
;
;------------------------------------------------------------------------

;       Register Addresses

M_IPR   equ     x:$FFFF           ; Interrupt Priority Register

;       Interrupt Priority Register Bit Flags

M_IAL   equ     $7              ; IRQA Mode Mask
M_IAL0  equ     0               ; IRQA Mode Interrupt Priority Level (low)
M_IAL1  equ     1               ; IRQA Mode Interrupt Priority Level (high)
M_IAL2  equ     2               ; IRQA Mode Trigger Mode
M_IBL   equ     $38             ; IRQB Mode Mask
M_IBL0  equ     3               ; IRQB Mode Interrupt Priority Level (low)
M_IBL1  equ     4               ; IRQB Mode Interrupt Priority Level (high)
M_IBL2  equ     5               ; IRQB Mode Trigger Mode
M_HPL   equ     $C00            ; Host Interrupt Priority Level Mask
M_HPL0  equ     10              ; Host Interrupt Priority Level Mask (low)
M_HPL1  equ     11              ; Host Interrupt Priority Level Mask (high)
M_SSL   equ     $3000           ; SSI Interrupt Priority Level Mask
M_SSL0  equ     12              ; SSI Interrupt Priority Level Mask (low)
M_SSL1  equ     13              ; SSI Interrupt Priority Level Mask (high)
M_SCL   equ     $C000           ; SCI Interrupt Priority Level Mask
M_SCL0  equ     14              ; SCI Interrupt Priority Level Mask (low)
M_SCL1  equ     15              ; SCI Interrupt Priority Level Mask (high)

;------------------------------------------------------------------------
;
;       EQUATES for I/O Port Programming
;
;------------------------------------------------------------------------

;       Register Addresses

m_bcr   equ     x:$FFFE           ; Port A Bus Control Register
m_pbc   equ     x:$FFE0           ; Port B Control Register
m_pbddr equ     x:$FFE2           ; Port B Data Direction Register
m_pbd   equ     x:$FFE4           ; Port B Data Register
m_pcc   equ     x:$FFE1           ; Port C Control Register
m_pcddr equ     x:$FFE3           ; Port C Data Direction Register
m_pcd   equ     x:$FFE5           ; Port C Data Register

;	Port C Control Register Bit Flags

m_std	equ	8
m_srd	equ	7
m_sck	equ	6
m_sc2	equ	5
m_sc1	equ	4
m_sc0	equ	3
m_sclK	equ	2
m_txd	equ	1
m_rxd	equ	0


;------------------------------------------------------------------------
;
;       EQUATES for Host Interface
;
;------------------------------------------------------------------------

;       Register Addresses

m_hcr   equ     x:$FFE8           ; Host Control Register
m_hsr   equ     x:$FFE9           ; Host Status Register
m_hrx   equ     x:$FFEB           ; Host Receive Data Register
m_htx   equ     x:$FFEB           ; Host Transmit Data Register

;       Host Control Register Bit Flags

m_hrie  equ     0               ; Host Receive Interrupt Enable
m_htie  equ     1               ; Host Transmit Interrupt Enable
m_hcie  equ     2               ; Host Command Interrupt Enable
m_hf2   equ     3               ; Host Flag 2
m_hf3   equ     4               ; Host Flag 3

;       Host Status Register Bit Flags

m_hrdf  equ     0               ; Host Receive Data Full
m_htde  equ     1               ; Host Transmit Data Empty
m_hcp   equ     2               ; Host Command Pending
m_hf    equ     $18             ; Host Flag Mask
m_hf0   equ     3               ; Host Flag 0
m_hf1   equ     4               ; Host Flag 1
m_dma   equ     7               ; DMA Status

;------------------------------------------------------------------------
;
;       EQUATES for Serial Communications Interface (SCI)
;
;------------------------------------------------------------------------

;       Register Addresses

m_srxl  equ     x:$FFF4           ; SCI Receive Data Register (low)
m_srxm  equ     x:$FFF5           ; SCI Receive Data Register (middle)
m_srxh  equ     x:$FFF6           ; SCI Receive Data Register (high)
m_stxl  equ     x:$FFF4           ; SCI Transmit Data Register (low)
m_stxm  equ     x:$FFF5           ; SCI Transmit Data Register (middle)
m_stxh  equ     x:$FFF6           ; SCI Transmit Data Register (high)
m_stxa  equ     x:$FFF3           ; SCI Transmit Data Address Register
m_scr   equ     x:$FFF0           ; SCI Control Register
m_ssr   equ     x:$FFF1           ; SCI Status Register
m_sccr  equ     x:$FFF2           ; SCI Clock Control Register

;       SCI Control Register Bit Flags

m_wds   equ     $3              ; Word Select Mask
m_wds0  equ     0               ; Word Select 0
m_wds1  equ     1               ; Word Select 1
m_wds2  equ     2               ; Word Select 2
m_sbk   equ     4               ; Send Break
m_wake  equ     5               ; Wake-up Mode Select
m_rwi   equ     6               ; Receiver Wake-up Enable
m_woms  equ     7               ; Wired-OR Mode Select
m_re    equ     8               ; Receiver Enable
m_te    equ     9               ; Transmitter Enable
m_ilie  equ     10              ; Idle Line Interrupt Enable
m_rie   equ     11              ; Receive Interrupt Enable
m_tie   equ     12              ; Transmit Interrupt Enable
m_tmie  equ     13              ; Timer Interrupt Enable

;       SCI Status Register Bit Flags

m_trne  equ     0               ; Transmitter Empty
m_tdre  equ     1               ; Transmit Data Register Empty
m_rdrf  equ     2               ; Receive Data Register Full
m_idle  equ     3               ; Idle Line
m_or    equ     4               ; Overrun Error
m_pe    equ     5               ; Parity Error
m_fe    equ     6               ; Framing Error
m_r8    equ     7               ; Received Bit 8

;       SCI Clock Control Register Bit Flags

m_cd    equ     $FFF            ; Clock Divider Mask
m_cod   equ     12              ; Clock Out Divider
m_scp   equ     13              ; Clock Prescaler
m_rcm   equ     14              ; Receive Clock Source
m_tcm   equ     15              ; Transmit Clock Source

;------------------------------------------------------------------------
;
;       EQUATES for Synchronous Serial Interface (SSI)
;
;------------------------------------------------------------------------

;       Register Addresses

m_rx    equ     x:$FFEF           ; Serial Receive Data Register
m_tx    equ     x:$FFEF           ; Serial Transmit Data Register
m_cra   equ     x:$FFEC           ; SSI Control Register A
m_crb   equ     x:$FFED           ; SSI Control Register B
m_sr    equ     x:$FFEE           ; SSI Status Register
m_tsr   equ     x:$FFEE           ; SSI Time Slot Register

;       SSI Control Register A Bit Flags

m_pm    equ     $FF             ; Prescale Modulus Select Mask
m_dc    equ     $1F00           ; Frame Rate Divider Control Mask
m_dc0	equ	8
m_dc1	equ	9
m_dc2	equ	10
m_dc3	equ	11
m_dc4	equ	12
m_wl    equ     $6000           ; Word Length Control Mask
m_wl0   equ     13              ; Word Length Control 0
m_wl1   equ     14              ; Word Length Control 1
m_psr   equ     15              ; Prescaler Range

;       SSI Control Register B Bit Flags

m_of    equ     $3              ; Serial Output Flag Mask
m_of0   equ     0               ; Serial Output Flag 0
m_of1   equ     1               ; Serial Output Flag 1
m_scd   equ     $1C             ; Serial Control Direction Mask
m_scd0  equ     2               ; Serial Control 0 Direction
m_scd1  equ     3               ; Serial Control 1 Direction
m_scd2  equ     4               ; Serial Control 2 Direction
m_sckd  equ     5               ; Clock Source Direction
m_fsl   equ     8               ; Frame Sync Length
m_syn   equ     9               ; Sync/Async Control
m_gck   equ     10              ; Gated Clock Control
m_mod   equ     11              ; Mode Select
m_ste   equ     12              ; SSI Transmit Enable
m_sre   equ     13              ; SSI Receive Enable
m_stie  equ     14              ; SSI Transmit Interrupt Enable
m_srie  equ     15              ; SSI Receive Interrupt Enable

;       SSI Status Register Bit Flags

m_if    equ     $2              ; Serial Input Flag Mask
m_if0   equ     0               ; Serial Input Flag 0
m_if1   equ     1               ; Serial Input Flag 1
m_tfs   equ     2               ; Transmit Frame Sync
m_rfs   equ     3               ; Receive Frame Sync
m_tue   equ     4               ; Transmitter Underrun Error
m_roe   equ     5               ; Receiver Overrun Error
m_tde   equ     6               ; Transmit Data Register Empty
m_rdf   equ     7               ; Receive Data Register Full

;------------------------------------------------------------------------
;
;       EQUATES for Exception Processing
;
;------------------------------------------------------------------------

;       Register Addresses

m_ipr   equ     x:$FFFF           ; Interrupt Priority Register

;       Interrupt Priority Register Bit Flags

m_ial   equ     $7              ; IRQA Mode Mask
m_ial0  equ     0               ; IRQA Mode Interrupt Priority Level (low)
m_ial1  equ     1               ; IRQA Mode Interrupt Priority Level (high)
m_ial2  equ     2               ; IRQA Mode Trigger Mode
m_ibl   equ     $38             ; IRQB Mode Mask
m_ibl0  equ     3               ; IRQB Mode Interrupt Priority Level (low)
m_ibl1  equ     4               ; IRQB Mode Interrupt Priority Level (high)
m_ibl2  equ     5               ; IRQB Mode Trigger Mode
m_hpl   equ     $C00            ; Host Interrupt Priority Level Mask
m_hpl0  equ     10              ; Host Interrupt Priority Level Mask (low)
m_hpl1  equ     11              ; Host Interrupt Priority Level Mask (high)
m_ssl   equ     $3000           ; SSI Interrupt Priority Level Mask
m_ssl0  equ     12              ; SSI Interrupt Priority Level Mask (low)
m_ssl1  equ     13              ; SSI Interrupt Priority Level Mask (high)
m_scl   equ     $C000           ; SCI Interrupt Priority Level Mask
m_scl0  equ     14              ; SCI Interrupt Priority Level Mask (low)
m_scl1  equ     15              ; SCI Interrupt Priority Level Mask (high)

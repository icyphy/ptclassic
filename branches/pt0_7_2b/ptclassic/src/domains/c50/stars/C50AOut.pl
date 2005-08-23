defstar {
	name { AOut }
	domain { C50 }
	desc { An output star for the DSK320 analog output port }
	version { @(#)C50AOut.pl	1.5	10/24/96 }
	author { Luis Gutierrez, A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<p>
This star is an interrupt driven D/A star for the DSK320 board.
The output voltage at the RCA connector is scaled to maximum 3V.
The sampling rate is approximated as 285.7KHz/N where N is a
positive integer.  Thus, setting the sampleRate parameter to
8000 Hz will yield a sample rate of 7936 Hz(N = 36).
<p>
If "interruptBufferSize" is left as -1 the star will automatically allocate
an interrupt buffer of size rep where rep is the number of times the
star is repeated on the schedule except when rep is 1, in which case no
buffer will be allocated.   If "interruptBufferSize" is set to 0 or 1 no 
buffer will be allocated;  otherwise the size of the buffer is set to the 
value of "interruptBufferSize".
<p>
On each firing this star will copy its input into the interrupt  buffer
if it is not full.  If the interrupt buffer is full it will wait until
it is empty once again. If no buffer is allocated, the star will wait
until it can send its input to the A\D converter.
<p>
	}

	seealso { AIn }
        
	ccinclude { "TITarget.h" }
 	
	input {
		name {input}
		type {FIX}
		attributes { P_NOINIT }
	}

	state {
		name { sampleRate }
		type { int }
		desc {
The sampling rate of this star(see explanation)
		}
		default { "8000" }
	}

	state {
		name { interruptBufferSize }
		type { int }
		desc {
-1 = set buffer size automatically; 0 = no buffer,  otherwise allocate
an interrupt buffer of this size.
		}
		default {" -1" }
	}

	state {
		name { xmitBuffer }
		type { fixarray }
  		desc { the array contains samples which will be output  }
		default { "0.0" }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_BMEM }
	}

	state {
		name { temp }
		type { intarray }
		desc { holds pointers to buffers; counters }
		default { 0 }
		attributes {A_NONSETTABLE|A_BMEM|A_CONSEC|A_NOINIT }
	}

	state {
		name { bufferFullFlag }
		type { int }
		desc { flag that's != 0 if tempBuffer is full; 0 otherwise}
		default { 0 }
		attributes {A_NONSETTABLE|A_BMEM|A_NOINIT }
	}

	state {
		name { tempBuffer }
		type { fixarray }
  		desc { temporary storage for output samples  }
		default { "0.0" }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_BMEM }
	}

	protected{
		int	bufSize;
		int	tb;
	}


	setup {

		tb = int(  285714.286/double(sampleRate) + 0.5);

	}

		

	begin {
		bufSize = int(interruptBufferSize);
		int numReps = this->reps();
		
		if (bufSize < 0) {
		bufSize = numReps;
		}
		
		if (bufSize < 2) bufSize = 0;

		if (bufSize > 0) {
			xmitBuffer.resize(bufSize);
			tempBuffer.resize(bufSize);
			temp.resize(7);
		} else {

		// a buffer size of 1 is equiv to just wait until
		// the data is transferred to the AIC so its the same
		// as no buffer at all.  If we don't use a buffer then
		// we don't need to allocate memory for xmitBuffer,
		// tempBuffer, temp

			xmitBuffer.clearAttributes(A_BMEM);
			tempBuffer.clearAttributes(A_BMEM);
			temp.clearAttributes(A_BMEM);
			temp.clearAttributes(A_CONSEC);
		}	
		

	}	



	codeblock(initInput){
	.ds	$addr(input)
$starSymbol(AO)	.word	0
	}
	
	codeblock(initPtrs,""){
	.ds	$addr(temp)
$starSymbol(XPTR)	.word	$addr(xmitBuffer)
$starSymbol(TPTR)	.word	$addr(tempBuffer)
$starSymbol(PTR1)	.word	$addr(xmitBuffer)
$starSymbol(PTR2)	.word	$addr(tempBuffer)
$starSymbol(CNTR1)	.word	@(int(bufSize))
$starSymbol(CNTR2)	.word	@(int(bufSize))
$starSymbol(TMP)	.word	0	
	}

	codeblock(flagAddress){
	.ds	$addr(bufferFullFlag)
	}

	codeblock(initFlag){
$starSymbol(AOF)	.word	0
	.text
	}


	initCode{
		addCode(xmitSetup(),"TISubProcs");
		addCode(initInput);
		if (bufSize){
			addCode(initPtrs());
			addCode(interruptStd(),"TISubProcs");
		} else{	
			addCode(flagAddress);
			addCode(interruptOne,"TISubProcs");
		}
		addCode(initFlag);

                TITarget * mytarget = (TITarget *) this->target();
		mytarget->setFlag("xmitFlag ");

	}		


	codeblock(xmitSetup,""){
SETUPX:
	ldp	#TB			
	splk	#@(int(tb)),TB		; setup TB reg
	ldp	#00h
	opl	#022h,IMR		; enable xmit interrupt
	ret
}

	codeblock(interruptOne){
XINT:
	ldp	#$starSymbol(AO)	; get addr of input
	lacc	$starSymbol(AO)
	and	#0ffffh,2		; clear 2LSB
	samm	dxr			; output result
	ldp	#$starSymbol(AOF)	; 
	splk	#0h,$starSymbol(AOF)	; clear bufferFullFlag
	rete				; return and enable interrupts
	}


	codeblock(interruptStd,""){
XINT:
	ldp     #$starSymbol(XPTR)
	sar	ar1,$starSymbol(TMP)                          
	mar     *,ar1
	lar	ar1,$starSymbol(XPTR)      
	lacc    *+,0                     
	and     #0ffffh,2		; clear 2 LSB            
	samm    dxr			; output result
	lacc    $starSymbol(CNTR2)                
	sub     #1
	bcndd   $label(xEnd),gt       
	sar	ar1,$starSymbol(XPTR)                           
	sacl    $starSymbol(CNTR2)
	lph     $starSymbol(PTR2)	; when xmitBuffer becomes empty
	dmov    $starSymbol(PTR1)	; exchange it with tempBuffer
	sph     $starSymbol(PTR1)
	bldd	$starSymbol(PTR1),#$addr(temp)
	bldd    $starSymbol(PTR2),#$addr(temp,1)
	splk    #@(int(bufSize)),$starSymbol(CNTR1)
	dmov    $starSymbol(CNTR1)	; reset counters and 
	apl     #0h,$starSymbol(AOF)	; bufferFull flag
$label(xEnd)
	lar     ar1,$starSymbol(TMP)
	rete
	}

	codeblock(fireStd,""){
$label(check):
	ldp     #$starSymbol(XPTR)
	lacc    $starSymbol(AOF),0
	bcndd   $label(cont),eq
	lar     ar2,$starSymbol(TPTR)
	mar     *,ar2
	idle                 
	b       $label(check)
$label(cont)
	bldd    #$addr(input),*+
	lacc    $starSymbol(CNTR1)
	sub     #1
	sacl    $starSymbol(CNTR1)
	sar	ar2,$starSymbol(TPTR)  
	xc      2,LEQ                  
	opl     #1h,$starSymbol(AOF)
	ldp     #00h
	}
	

//FIXME(?): In fireOne i could have used a bcndd and put the 
//splk after but i don't know if interrupts can trap in the next two
//instructions following the bcndd. If they can't it's better to use
//a bcndd and put the splk after

	codeblock(fireOne){
$label(check):
	ldp	#$starSymbol(AOF)
	lacc	$starSymbol(AOF)
	bcnd	$label(cont),eq
	idle
	b	$label(check)
$label(cont)
	splk	#01h,$starSymbol(AOF)	; set bufferFullFlag
	ldp	#00h			; clear dp
	}

	go {
	
		if (bufSize) 
			addCode(fireStd());
		else
			addCode(fireOne);	
	
	}
	execTime {
		return 3;
	}
}    








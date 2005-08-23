defstar {
	name { AIn }
	domain { C50 }
	desc { An input star for the DSK320 analog input port }
	version { @(#)C50AIn.pl	1.5	10/24/96 }
	author { A. Baensch , ported from Gabriel }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }

	htmldoc {
<p>
This star is an interrupt driven A/D star for the DSK320 board.
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
	}

	seealso { AOut }
        
	ccinclude { "TITarget.h" }
 	
	output {
		name {output}
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
		name { rcvBuffer }
		type { fixarray }
  		desc { the array contains samples which will be input  }
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
  		desc { temporary storage for input samples  }
		default { "0.0" }
 		attributes { A_NONSETTABLE|A_NONCONSTANT|A_BMEM }
	}

	protected{
		int	bufSize;
		int	rb;
	}


	setup {

		rb = int(  285714.286/double(sampleRate) + 0.5);

	}

		

	begin {
		bufSize = int(interruptBufferSize);
		int numReps;
		
		if (bufSize < 0) {
		numReps = this->reps();
		}
		
		if (bufSize < 2) bufSize = 0;

		if (bufSize > 0) {
			rcvBuffer.resize(bufSize);
			tempBuffer.resize(bufSize);
			temp.resize(7);
		} else {

		// a buffer size of 1 is equiv to just wait until
		// the data is transferred to the AIC so its the same
		// as no buffer at all.  If we don't use a buffer then
		// we don't need to allocate memory for rcvBuffer,
		// tempBuffer, temp

			rcvBuffer.clearAttributes(A_BMEM);
			tempBuffer.clearAttributes(A_BMEM);
			temp.clearAttributes(A_BMEM);
			temp.clearAttributes(A_CONSEC);
		}	
		

	}	



	codeblock(initOutput){
	.ds	$addr(output)
$starSymbol(AI)	.word	0
	}
	
	codeblock(initPtrs,""){
	.ds	$addr(temp)
$starSymbol(RPTR)	.word	$addr(rcvBuffer)
$starSymbol(TPTR)	.word	$addr(tempBuffer)
$starSymbol(PTR1)	.word	$addr(rcvBuffer)
$starSymbol(PTR2)	.word	$addr(tempBuffer)
$starSymbol(CNTR1)	.word	@(int(bufSize))
$starSymbol(CNTR2)	.word	@(int(bufSize))
$starSymbol(TMP)	.word	0	
	}

	codeblock(flagAddress){
	.ds	$addr(bufferFullFlag)
	}

	codeblock(initFlag){
$starSymbol(AIF)	.word	0
	.text
	}


	initCode{
		addCode(rcvSetup(),"TISubProcs");
		addCode(initOutput);
		if (bufSize){
			addCode(initPtrs());
			addCode(interruptStd(),"TISubProcs");
		} else{	
			addCode(flagAddress);
			addCode(interruptOne,"TISubProcs");
		}
		addCode(initFlag);

                TITarget * mytarget = (TITarget *) this->target();
		mytarget->setFlag("rcvFlag ");

	}		


	codeblock(rcvSetup,""){
SETUPR:
	ldp	#RB			
	splk	#@(int(rb)),RB		; setup RB reg
	ldp	#00h
	opl	#012h,IMR		; enable rcv interrupt
	ret
}

	codeblock(interruptOne){
RINT:
	smmr	drr,#$addr(output)	; store received sample in output
	ldp	#$starSymbol(AIF)	; 
	splk	#1h,$starSymbol(AIF)	; set bufferFullFlag
	rete				; return and enable interrupts
	}


	codeblock(interruptStd,""){
RINT:
	ldp     #$starSymbol(RPTR)
	sar	ar1,$starSymbol(TMP)                          
	mar     *,ar1
	lar	ar1,$starSymbol(RPTR)      
	lamm	drr			; 
	sacl	*+			; store in buffer
	lacc    $starSymbol(CNTR2)                
	sub     #1
	bcndd   $label(xEnd),gt       
	sar	ar1,$starSymbol(RPTR)                           
	sacl    $starSymbol(CNTR2)
	lph     $starSymbol(PTR2)	; when rcvBuffer becomes empty
	dmov    $starSymbol(PTR1)	; exchange it with tempBuffer
	sph     $starSymbol(PTR1)
	bldd	$starSymbol(PTR1),#$addr(temp)
	bldd    $starSymbol(PTR2),#$addr(temp,1)
	splk    #@(int(bufSize)),$starSymbol(CNTR1)
	dmov    $starSymbol(CNTR1)	; reset counters and 
	opl     #01h,$starSymbol(AIF)	; set bufferFull flag
$label(xEnd)
	lar     ar1,$starSymbol(TMP)
	rete
	}

	codeblock(fireStd,""){
$label(check):
	ldp     #$starSymbol(RPTR)
	lacc    $starSymbol(AIF),0
	bcndd   $label(cont),neq
	lar     ar2,$starSymbol(TPTR)
	mar     *,ar2
	idle                 
	b       $label(check)
$label(cont)
	bldd    *+,#$addr(output)
	lacc    $starSymbol(CNTR1)
	sub     #1
	sacl    $starSymbol(CNTR1)
	sar	ar2,$starSymbol(TPTR)  
	xc      1,LEQ                  
	sach	$starSymbol(AIF)
	ldp     #00h
	}
	

//FIXME(?): In fireOne i could have used a bcndd and put the 
//splk after but i don't know if interrupts can trap in the next two
//instructions following the bcndd. If they can't it's better to use
//a bcndd and put the splk after

	codeblock(fireOne){
$label(check):
	ldp	#$starSymbol(AIF)
	lacc	$starSymbol(AIF)
	bcnd	$label(cont),neq
	idle
	b	$label(check)
$label(cont)
	splk	#00h,$starSymbol(AIF)	; set bufferFullFlag
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









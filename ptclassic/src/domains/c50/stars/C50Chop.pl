defstar {
	name {Chop}
	domain {C50}
	desc {
On each execution, this star reads a block of "nread" particles
and writes them to the output with the given offset.  The number of
particles written is given by "nwrite".  The output block contains
all or part of the input block, depending on "offset" and "nwrite".
The "offset" specifies where in the output block the first (oldest)
particle in the input block will lie.  If "offset" is positive,
then the first "offset" output particles will be either particles
consumed on previous firings (if "use_past_inputs" parameter is YES),
or zero (otherwise).  If "offset" is negative, then the first "offset"
input particles will be discarded.
	}
	version { $Id$}
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
See the explanation of the SDFChop star.
	}
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{=input}
	}
	state {
		name {nread}
		type {int}
		default {128}
		desc {Number of particles read.}
	}
	state {
		name {nwrite}
		type {int}
		default {64}
		desc {Number of particles written.}
	}
	state {
		name {offset}
		type {int}
		default {0}
		desc { Position of output block relative to input block.}
	}
	
	state {
		name { use_past_inputs }
		type { int }
		default {"YES"}
		desc {
if offset > 0, specify whether to use previously read inputs
(otherwise use zeros).
		}
	}

	state {
		name { pastInputs }
		type { intarray}
		default {" 0 0"}
		desc { internal storage for past inputs }
		attributes{ A_NONSETTABLE|A_CIRC|A_UMEM }
	}
	
	state{
		name {pastInputsPtr }
		type { int }
		default { 0 }
		desc { it's were the next input goes in pastInputs }
		attributes { A_NONSETTABLE|A_UMEM }
	}

	state{
		name { fill }
		type { fix }
		default { 0.0 }
		desc { Value of fill particles }
		attributes { A_CONSTANT|A_NONSETTABLE }
	}

	protected {
		int hiLim, index, padding, numInputs, numCpy, cmplx;
		int read,write,off;
		int fillAsInt;
//hiLim : start writing trailing zeros at this point
//index : copy inputs starting at inputs[index]
//padding: number of old inputs or zeros to write at beginning
//numInputs: number of inputs to copy
//numCpy: number of inputs to store in pastInputs buffer
//read: effective num of inputs to read(to allow for complex numbers)
//write: effective num of inputs to copy
//off: effective offset
//fillAsInt: value of fill as an integer that the dsk assembler can handle
	}

	method {
	    name { computeRange }
	    type { "void" }
	    arglist { "()" }
	    access { protected }
	    code {
		// Compute the range of output indexes that come from inputs
		// This method is called in the setup() method for the Chop
		// star, and in the go method for ChopVarOffset because
		// it resets the offset parameter
		hiLim = read + off;
		if (hiLim > write) hiLim = write;
		numInputs = read;
		padding = off;
		if (padding < 0 ) {
			padding = 0;
			numInputs = hiLim;
		}
		if (padding > write){
			padding = write;
		}
		if ((write - padding) < numInputs) {
			numInputs = write - padding;
		}
		index = -off;
		if (index <  0) {
			index = 0;
		} else if (index > read){
			Error::abortRun(*this,
					"absolute value of negative offsets ",
					"must not be larger than nread");
			return;
		}
		numCpy = read;
		if (numCpy > padding) numCpy = padding;
	    }
	}

	initCode{
		addCode(initPtr);
	}

	setup{
		if (int(nread) <= 0){
			Error::abortRun(*this,
					"The number of samples to read ",
					"must be positive");
			return;
		}
		if (int(nwrite) <= 0){
			Error::abortRun(*this,
					"The number of samples to write ",
					"must be positive");
			return;
		}
		cmplx = 0;
		if (input.resolvedType() == COMPLEX){
		//complex inputs take twice as much space
			read = 2*(int(nread));
			write = 2*(int(nwrite));
			off = 2*(int(offset));
			cmplx = 1;
		} else {
			read = int(nread);
			write = int(nwrite);
			off = int(offset);
		}

		input.setSDFParams(int(nread),int(nread)-1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);
		computeRange();
		// FIXME: We had to use a past_inputs state because
		// the C50 domain does not support a construct like
		// input.setSDFParams(int(nread), int(nread)+int(offset)-1);
		if (int(use_past_inputs) > 0) {
			pastInputs.resize(padding);
		}
		
		//calc value of fill as an unsigned integer
		
		double temp = fill.asDouble();
		if (temp >= 0) {
			fillAsInt = int(32768*temp + 0.5);
		} else {
			fillAsInt = int(32768*(2+temp) + 0.5);
		}
	}

	go {
		if ( padding > 0){
			if (int(use_past_inputs)){
				int save_indx = read - padding;
				if (save_indx < 0) save_indx = 0;
				addCode(padNsavePastInputs(save_indx));
			} else {
				addCode(loadPaddingValue());
				addCode(padWithNumberStart());
			}
		} else addCode(loadPaddingValue());
		if (numInputs > 0) addCode(writeInputToOutput(index));
		if (hiLim < write){
			int pad_end = write- hiLim;
			addCode(padWithNumberEnd(pad_end));
		}	
	}

	codeblock(initPtr){
	.ds	$addr(pastInputsPtr)
	.word	$addr(pastInputs)
	.text
	}

	codeblock(padNsavePastInputs,"int saveindx"){
*initialize circular buffer for pastInputs
	lacc	#$addr(pastInputs)	; get start addr of circ buffer
	samm	cbsr1			; set start addr of circ buffer
	add	#@(int(padding) - 1)	; get end addr of circ buffer
	samm	cber1			; set end addr of circ buffer
	lacl	#8h			
	samm	cbcr				; enable circ buffer 1 with ar0
	lmmr	ar0,#$addr(pastInputsPtr)	; get loc of next input
	lar	ar1,#$addr(output)		; get loc of output
	mar	*,ar0
	rpt	#@(int(padding)-1)		
	bldd	*+,#$addr(output)		; copy last inputs to output
	rpt	#@(int(numCpy)-1)		
	bldd	#$addr(input,@saveindx),*+	; save current inputs
	smmr	ar0,#$addr(pastInputsPtr)	; save ptr for next time
	zap
	samm	brcr				; clear circ buffer reg.
	}

	
	codeblock(loadPaddingValue,""){
	lacc	#@fillAsInt
	}

	codeblock(padWithNumberStart,""){
	lar	ar0,#$addr(output)
	mar	*,ar0
	rpt	#@(int(padding)-1)
	sacl	*+
	}

	codeblock(writeInputToOutput,"int idx"){
	lar	ar0,#$addr(output,@padding)
	rpt	#@(int(numInputs)-1)
	bldd	#$addr(input,@idx),*+
	}

	codeblock(padWithNumberEnd,"int iter"){
	lar	ar0,#$addr(output,@(int(hiLim)))
	rpt	#@(iter - 1)
	sacl	*+
	}

	execTime {
		int time = 0;
		if ( padding > 0){
			if (int(use_past_inputs)){ 
				time += 14 + padding + numCpy;
			} else time += 5;
		}
		time += 2 + numInputs;
		if (hiLim < write) time += 2 + (write - hiLim);
		return time;
	}

}










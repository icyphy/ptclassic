defstar {
	name { Chop }
	domain { CG56 }
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
	version { @(#)CG56Chop.pl	1.6	01 Oct 1996 }
	author { Luis Javier Gutierrez and Edward A. Lee }
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
See the explanation of the SDFChop star.
	}
	
	input{
		name{ input }
		type {ANYTYPE}
	}
	output{
		name{ output }
		type {=input}
	}
	state{
		name { nread }
		type { int }
		default { 128 }
		desc { Number of particles read.}
	}
	state{
		name { nwrite }
		type { int }
		default { 64 }
		desc { Number of particles written.}
	}
	state {
		name { offset }
		type { int }
		default { 0 }
		desc { start output block relative to start of input block. }
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
	
	state{
		name{ past_inputs }
		type { IntArray }
		default { "0" }
		desc { internal state }
		attributes { A_NONSETTABLE|A_YMEM }
	}

	protected {
		int hiLim, index, padding, numInputs;
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
		hiLim = int(nread) + int(offset);
		if (hiLim > int(nwrite)) hiLim = int(nwrite);
		numInputs = int(nread);
		padding = int(offset);
		if (padding < 0 ) {
			padding = 0;
			numInputs = hiLim;
		}
		if (padding > int(nwrite)){
			padding = int(nwrite);
		}
		if ((int(nwrite) - padding) < numInputs) {
			numInputs = int(nwrite) - padding;
		}
		index = -int(offset);
		if (index < 0) {
			index = 0;
		}
		else if (index > int(nread)){
			Error::abortRun(*this,
					"absolute value of negative offsets ",
					"must not be larger than nread");
			return;
		}
	    }
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

		input.setSDFParams(int(nread),int(nread)-1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);
		computeRange();
		// FIXME: We had to use a past_inputs state because
		// the CG56 domain does not support a construct like
		// input.setSDFParams(int(nread), int(nread)+int(offset)-1);
		if (int(use_past_inputs) > 0) {
			past_inputs.resize(padding);
		}
	}

	go {
		addCode(initRegisters(index));
		if (padding > 0) {
			if (int(use_past_inputs)) { 
				int save_index = 0;
				if (save_index < 0)
					save_index = 0;
				else if (save_index > int(nread))
					save_index = int(nread);
				else
					save_index = int(nread) - padding;

				addCode(padAndSaveInputs(padding, save_index));
			}
			else {
				addCode(padWithZeroes(padding));
			}
		}
		if (numInputs > 0) {
			addCode(read_to_write(numInputs));
		}
		if (hiLim < int(nwrite)) {
			int pad_end = int(nwrite) - hiLim;
			addCode(padWithZeroes(pad_end));
		}
	}

	codeblock(initRegisters,"int x"){
; Register usage:
; r0 = address of the input sample (initialized to start of input buffer)
; r1 = address of the output sample (initialized to start of output buffer)
	move #($addr(input)+(@x)),r0
	move #$addr(output),r1	
	move #0,x0
	}
	codeblock(padWithZeroes,"int N"){
	do	#@N,$label(loop)	
	move	x0,$mem(output):(r1)+
$label(loop)
	}
	codeblock(padAndSaveInputs,"int N, int loc"){
	move	#$addr(past_inputs),r4
	move	#($addr(input)+@loc),r3
	do	#@N,$label(loop2)
	tst	b	$mem(input):(r3)+,a	y:(r4),b
	tst	b	b,$mem(output):(r1)+	a,y:(r4)+
$label(loop2)
	}
	codeblock(read_to_write,"int N"){
	do	#@N,$label(loop3)	
	move	$mem(input):(r0)+,a
	move	a,$mem(output):(r1)+
$label(loop3)
	}

	exectime {
		int time = 3;
		computeRange();

		if (int(use_past_inputs)) time += ( padding + 3); 
		else time += ( 5 + 3*padding );

		if (numInputs > 0) time += ( 3 + 2*numInputs);

		if (hiLim < int(nwrite)) {
			int pad_end = int(nwrite) - hiLim;
			time += (pad_end + 3);
		}

		return time;
	}
}

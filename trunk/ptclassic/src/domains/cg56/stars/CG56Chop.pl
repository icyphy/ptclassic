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
	version { $Id$ }
	author { Luis Javier Gutierrez and Edward A. Lee }
	copyright{
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an disclaimer of warranty provisions.
	}
	location { CG56 main library }
	explanation {
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
		type {int}
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
                int hiLim, index, padding;
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

		padding = int(offset);
		if (padding < 0 ) padding = 0;

		index = -int(offset);
		if (index < 0) index = 0;
		else if (index >= int(nread)) index = int(nread)-1;
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

		// FIXME: We had to use a past_inputs state because
		// the CG56 domain does not support a construct like
		// input.setSDFParams(int(nread),int(nread)+int(offset)-1);
		if (int(use_past_inputs) && int(offset) > 0) { 
			past_inputs.resize(int(offset));
		} 
		input.setSDFParams(int(nread),int(nread)-1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);

		computeRange();
	}

	go {
		addCode(init_registers(index));
		if (padding > 0) {
			if (int(use_past_inputs)) { 
				int save_index = (int(nread) - padding);
				addCode(pad_and_save_past_inputs(padding, save_index));
			}
			else {
				addCode(pad_with_zeroes(padding)); 
			}
		}
		addCode(read_to_write(hiLim));
		if (hiLim < int(nwrite)) {
			addCode(pad_with_zeroes(int(nwrite)-hiLim));
		}
	}

	codeblock(init_registers,"int x"){
; Register usage:
; r0 = address of the input sample (initialized to start of input buffer)
; r1 = address of the output sample (initialized to start of output buffer)
		move #($addr(input)+(@x)),r0
		clr	a	move #$addr(output),r1		; set a = 0
	}
	codeblock(pad_with_zeroes,"int N"){
		do	#@N,$label(loop)		; pad with @N zeroes
		move	a,$mem(output):(r1)+
$label(loop)
	}
	codeblock(pad_and_save_past_inputs,"int N, int loc"){
		move	#$addr(past_inputs),r4
		move	#($addr(input)+@loc),r3
		do	#@N,$label(loop2)
		tst	b	$mem(input):(r3)+,b	y:(r4),a
		tst	b	a,$mem(output):(r1)+	b,y:(r4)+
$label(loop2)
	}
	codeblock(read_to_write,"int N"){
		do	#@N,$label(loop3)		; copy input to output
		move	$mem(input):(r0)+,a
		move	a,$mem(output):(r1)+
$label(loop3)
	}

	exectime {
		computeRange();
		return 8;		// FIXME
	}
}

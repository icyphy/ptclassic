defstar {
	name { Chop }
	domain { CG56 }
	desc { }
	version {@(#)CG56Chop.pl 1.10 1/26/96 }
	author { Luis Gutierrez }
	copyright{
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { }
	explanation { }
	
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
		desc { if offset > 0, specify whether to use previously read inputs (otherwise use zeros).}
	}
	state{
		name{ past_inputs }
		type { IntArray }
		default { "0" }
		desc { internal state }
		attributes { A_NONSETTABLE|A_YMEM }
	}
	
	setup{
		if ((int)nread <= 0){
			Error::abortRun(*this, "The number of samples to read ",
							"must be positive");
			return;
		}
		if ((int)nwrite <= 0){
			Error::abortRun(*this, "The number of samples to write",
							"must be positive");
			return;
		}
		if (int(use_past_inputs)){
			if ((int)offset > 0){ 
				past_inputs.resize(int(offset));
			}
		} 
		input.setSDFParams(int(nread),int(nread) - 1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);
	}


	go{
		int hiLim = int(nread)+int(offset);
		if (hiLim > int(nwrite))  hiLim = int(nwrite);
	
		int padding = int(offset);
		if (padding < 0 ) padding = 0;

		int index = (-(int(offset)));
		if (index<0) index = 0;
		else if (index >= int(nread)) index=int(nread)-1;
		
		addCode(init_registers(index));
		if (padding > 0){
			if (int(use_past_inputs)){ 
				int save_index=( (int)nread - padding);
				addCode(pad_and_save_past_inputs(padding, save_index));
			}else 
				addCode(pad_with_zeroes(padding)); 
		}
		addCode(read_to_write(hiLim));
		if (hiLim < int(nwrite)) addCode(pad_with_zeroes(int(nwrite)-hiLim));
	}


	codeblock(init_registers,"int x"){
		move #($addr(input)+(@x)),r0 ; r0=init to start of input
		move #$addr(output),r1       ; r1=init to start of output
	}
	codeblock(pad_with_zeroes,"int N"){
		clr a
		do #@N,$label(loop)
		move a,$mem(output):(r1)+
$label(loop)
	}
	codeblock(pad_and_save_past_inputs,"int N, int loc"){
		move #$addr(past_inputs),r4
		move #($addr(input)+@loc),r3
		do #@N,$label(loop2)
		tst b     $mem(input):(r3)+,b  y:(r4),a
		tst b     a,$mem(output):(r1)+   b,y:(r4)+
$label(loop2)
	}
	codeblock(read_to_write,"int N"){
		do #@N,$label(loop3)
		move $mem(input):(r0)+,a
		move a,$mem(output):(r1)+
$label(loop3)
	}


}




	
	
							
		

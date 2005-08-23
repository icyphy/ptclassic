defstar {
	name { ChopVarOffset }
	domain { CG56 }
	desc { }
	version {@(#)CG56ChopVarOffset.pl 1.10 1/26/96 }
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
	input{
		name { offset }
		type { int }
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
		attributes { A_YMEM|A_NONCONSTANT }
	}
	state{
		name { nwrite }
		type { int }
		default { 64 }
		desc { Number of particles written.}
		attributes { A_YMEM|A_NONCONSTANT}
	}
	state{
		name{ one }
		type { int }
		default { 1 }
		attributes { A_YMEM|A_CONSTANT|A_NONSETTABLE }
		desc { 
temporary solution to a problem--will get rid of this when i rewrite the star
		     }
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
		input.setSDFParams(int(nread),int(nread) - 1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);
	}

	codeblock(chop){
		move $ref(offset),b
		clr a             b,x0                ;B=X0=offset          
		neg b             #$addr(output),r5   ;B=-offset
		cmp x0,a          $ref(nread),y0      ;Y0=nread
		tgt a,b                               ;if (offset > 0) B=0
		jle $label(skip_padding)              ;do does at leat 1
		do x0,$label(skip_padding)            ;iteration so if offset
		move a,$mem(output):(r5)+             ;is 0 one most skip this
$label(skip_padding) 
		add x0,a          $ref(one),x1        ;A=offset X1=1
		add y0,a          $ref(nwrite),x0     ;A=offset+readn=hiLim X0=nwrite
		cmp x0,a          $addr(input),y1
		tlt x0,a                              ;if (nwrite < hiLim) hiLim = nwrite
		cmp y0,b	  		      
		jgt $label(index_ok)                  ;if (index(=B)>= nread)
		move y0,b                             ; index=nread-1
		sub x1,b         
$label(index_ok)
		add y1,b                              ;
		move b,r0
		do a,$label(input_to_output)
		move $mem(input):(r0+n0)+,x1          ;copy input to output
		move x1,$mem(output):(r5)+
$label(input_to_output)
		sub x0,a  #$0,b			      ;A=hiLim-nwrite B=0
		cmp a,b                               ;if (hiLim-nwrite < 0)
		jge $label(skip_end_padding)
		neg a	                              ; A=nwrite-hiLim
		do a,$label(skip_end_padding)
		move b,$mem(output):(r5)+           
$label(skip_end_padding)
	 }
	go{
		addCode(chop);
	}
}
	
	
							
		

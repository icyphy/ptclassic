defstar {
	name { ChopVarOffset }
	derivedFrom { Chop }
	domain { CG56 }
	desc {
This star has the same functionality as the
Chop star except now the offset parameter is
determined at run time through a control input.
	}
	version { @(#)CG56ChopVarOffset.pl	1.2	2/27/96 }
	author { Luis Javier Gutierrez }
	copyright{
Copyright (c) 1990- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, an ddisclaimer of warranty provisions.
	}
	location { CG56 main library }

	input {
		name { offsetCntrl }
		type { int }
	}

	constructor {
		offset.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
		// nread and nwrite need to be allocated in memory.  one can't use
		// move #$val(nwrite),{x0,a,b} since this puts an 8 bit constant(the value of nwrite)
		// into the 8 MOST significant bits of {x0,a,b}. move $ref(nwrite),x0 
		//does the right thing.  
		nread.setAttributes(A_CONSTANT|A_YMEM);
		nwrite.setAttributes(A_CONSTANT|A_YMEM);
		use_past_inputs.setAttributes(A_NONCONSTANT|A_NONSETTABLE);
		use_past_inputs = FALSE;
	}

	// Inherit the setup method from the Chop star
	go{
		addCode(init_range);
		addCode(chop);
	}

	codeblock(init_range) {
; at the end of this block:  
; x0= # of 0's at begining; x1 = # of inputs written to output
; b = # of 0's at the end; r0 = address of first input written to output
; r5=address of first output
; if offset < 0 and |offset| > nread no inputs will be written
	move $ref(offsetCntrl),a
	move #0,x0
	move $ref(nread),x1                   ; X1=nread
	neg a         a,b                     ; A=-offset b=offset
	tst a         $ref(nwrite),y0         ;
	tlt x0,a                              ; if offset > 0 A=0 
	tgt x0,b                              ; if offset < 0 b=0 
	cmp x1,a                              ; check to see if num of inputs to skip > nread;
	tgt x1,a                              ; if so, num of inputs to skip = nread
	cmp y0,b     a,y1                     ; check to see if padding > nwrite --  Y1=num of inputs to skip
	tgt y0,b                              ; if so, padding = nwrite
	move b,x0                             ; x0=num of 0's to pad at beginning
; A=Y1=num of inputs to skip 
	move x1,b                             
	sub y1,b     y1,n0                    ; B=num of inputs that can be read(nread-num of inputs to skip)
; n0=num of inputs to skip
	move $ref(nwrite),a
	sub x0,a  (r0)+n0                     ; A=number of outputs that weren't padded with 0's(i.e writeable)
; r0=address of first input written to output
	cmp b,a     
	tge b,a                               ; A=number of inputs to read
	add x0,a     a,x1                     ; A=number of inputs to read + number of padding 0's
; X1= number of inputs to read
	move $ref(nwrite),b                   ; B=nwrite
	sub a,b      #$addr(output),r5         ; B=nwrite - num of inputs to be read - number of padding 0's
; = number of paddding 0's at the end	
	}
	
	codeblock(chop) {
; zero is stored in a
	clr a
	cmp x0,a     b,y0
	jge $label(skip_initial_padding)    ; do loops execute 1->65536 times but not 0
	do  x0,$label(skip_initial_padding) ; if x0 were 0 the loop would execute 65536 times!
	move a,$mem(output):(r5)+           ; insert 0's
$label(skip_initial_padding)
	cmp x1,a                            ; can't have x1 = 0 for same reason as above
	jge $label(skip_copying_input_to_output)
	do  x1,$label(skip_copying_input_to_output)
	move $mem(input):(r0)+,y1
	move y1,$mem(output):(r5)+
$label(skip_copying_input_to_output)
	cmp y0,a
	jge $label(skip_padding_at_end)
	do y0,$label(skip_padding_at_end)
	move a,$mem(output):(r5)+
$label(skip_padding_at_end)
	}

	exectime{
	//can't determine the execution time since the offset is variable
        //return the time to do condition checking(which is fixed) +
	//time to execute chop assuming no padding is done and (nwrite) inputs
	// are written
		return (21 + 13 + (int(nwrite)*2));
	}
}


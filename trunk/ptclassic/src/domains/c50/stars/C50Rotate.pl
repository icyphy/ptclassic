defstar {
	name { Rotate }
	domain { C50 }
	desc { Rotate a block of input data of N length  }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 control library }
	explanation {
.Id "reverse"
The star reads in an input block of length n and performs  a
circular  shift  of the input.  If rotation is positive, the
input is shifted to the left so that output[0] =
input[rotation],  output[1]  =  input[rotation+1],  etc.  If
rotation is negative, the input is shifted to the right so
that output[rotation]  =  input[0],  output[rotation+1]  =
input[1], etc.  The parameter rotation must be smaller  than
the parameter length in absolute value.
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}
        state  {
                name { rotation }
                type { int }
                default { 0 }
                desc {amount of shift to right (negative) or left (positive)}
        }
        state  {
                name { length }
                type { int }
                default { 16 }
                desc { length of input data }
        }
        state  {
                name { rotate }
                type { int }
                default { 0 }
                desc { length of input data }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }

	codeblock(greater) {
	mar	*,AR0				;
        lar     AR0,#$addr2(input,rotate)	;Address input,rotate	=> AR0
        splk    #$addr(output),BMAR		;Address output		=> BMAR
	rpt     #$val(length)-$val(rotate)-1	;for number of (length-rotate)
       	 bldd	*+,BMAR				;output(i) = input()	
        lar     AR0,#$addr(input)		;Address input		=> AR0
        rpt     #$val(rotate)-1			;for number of rotate
         bldd   *+,BMAR				;output(i) = input()
       	}
        codeblock(other) {
	mar    	*,AR0       			;
 	lar     AR0,#$addr(input)		;Address input		=> AR0
        splk    #$addr(output),BMAR		;Address output		=> BMAR
        rpt     #$val(length)			;for number of length
         bldd	*+,BMAR				;output(i) = input(i)
        }
        codeblock(one) {
        splk    #$addr(input),BMAR		;just move data from in to out
        bldd    BMAR,#$addr(output)		;
        } 

        setup {
                input.setSDFParams(int(length),int(length)-1);
                output.setSDFParams(int(length),int(length)-1);
        }
	go {
		int             i = rotation;

		if (i > length || -i > length)
		Error::abortRun(*this, ": Number of rotations > block length.");

		rotate = rotation;

		if (rotation < 0)
			rotate = (int)length + (int)rotation;

		if (rotate > 0 && length > 1)
			addCode(greater);
		else if (length > 1)
			addCode(other);
		else
			addCode(one);
	}
	exectime {
		if (length > 1 && rotation == 0)
			return 4 + 2 * int (length);
		else if (length == 1)
			return 3;
		else
			return 7 + 4 * int (length);
	}
}


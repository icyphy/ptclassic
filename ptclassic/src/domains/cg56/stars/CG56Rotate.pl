defstar {
	name { Rotate }
	domain { CG56 }
	desc { Rotate a block of input data of N length  }
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
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
; effective rotation: $val(rotate)
        move    #<$addr2(input,rotate),r0
        move    #<$addr(output),r1
        do      #$val(length)-$val(rotate),$label(lab2)
        move    x:(r0)+,a
        move    a,x:(r1)+
$label(lab2)
        move    #<$addr(input),r0
        do      #$val(rotate),$label(lab3)
        move    x:(r0)+,a
        move    a,x:(r1)+
$label(lab3)
	}
        codeblock(other) {
        move    #<$addr(input),r0
        move    #<$addr(output),r1
        do      #$val(length),$label(lab4)
        move    x:(r0)+,a
        move    a,x:(r1)+
$label(lab4)
        }
        codeblock(one) {
        move    $ref(input),a
        move    a,$ref(output)
        } 

        start {
                input.setSDFParams(int(length),int(length)-1);
                output.setSDFParams(int(length),int(length)-1);
        }
	go {
		int             i = rotation;

		if (i > length || -i > length)
		Error::abortRun(*this, ": Number of rotations > block length.");

		rotate = rotation;

		if (rotation < 0)
			rotate = length + rotation;

		if (rotate > 0 && length > 1)
			gencode(greater);
		else if (length > 1)
			gencode(other);
		else
			gencode(one);
	}
	exectime {
		if (length > 1 && rotation == 0)
			return 4 + 2 * int (length);
		else if (length == 1)
			return 2;
		else
			return 7 + 4 * int (length);
	}
}

defstar {
	name { Rotate }
	domain { CG56 }
	desc { Rotate a block of input data }
	version { @(#)CG56Rotate.pl	1.14 03/29/97 }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="reverse"></a>
The star reads in an input block of length <i>length</i> and performs a
circular shift of the input.
If <i>rotation</i> is positive, then the input is shifted to the left so
that output[0] = input[<i>rotation</i>], output[1] = input[<i>rotation</i> + 1],
etc.
If <i>rotation</i> is negative, then the input is shifted to the right so
that output[<i>rotation</i>] = input[0], output[<i>rotation</i> + 1] = input[1],
etc.
The parameter <i>rotation</i> must be smaller than the parameter
<i>length</i> in absolute value.
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
        move    $mem(input):(r0)+,a
        move    a,$mem(output):(r1)+
$label(lab2)
        move    #<$addr(input),r0
        do      #$val(rotate),$label(lab3)
        move    $mem(input):(r0)+,a
        move    a,$mem(outpu):(r1)+
$label(lab3)
	}
        codeblock(other) {
        move    #<$addr(input),r0
        move    #<$addr(output),r1
        do      #$val(length),$label(lab4)
        move    $mem(input):(r0)+,a
        move    a,$mem(outpu):(r1)+
$label(lab4)
        }
        codeblock(one) {
        move    $ref(input),a
        move    a,$ref(output)
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
			return 2;
		else
			return 7 + 4 * int (length);
	}
}

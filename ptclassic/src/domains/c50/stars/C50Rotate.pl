defstar {
	name { Rotate }
	domain { C50 }
	desc { Rotate a block of input data }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
.Id "reverse"
The star reads in an input block of length \fIlength\fR and performs a
circular shift of the input.
If \fIrotation\fR is positive, then the input is shifted to the left so
that output[0] = input[\fIrotation\fR], output[1] = input[\fIrotation\fR + 1],
etc.
If \fIrotation\fR is negative, then the input is shifted to the right so
that output[\fIrotation\fR] = input[0], output[\fIrotation\fR + 1] = input[1],
etc.
The parameter \fIrotation\fR must be smaller than the parameter
\fIlength\fR in absolute value.
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
		desc { internal length of input data }
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
		int i = rotation;

		if (i > int(length) || -i > int(length)) {
		    Error::abortRun(*this,
				    "Number of rotations > block length");
		    return;
		}

		rotate = rotation;
		if (int(rotation) < 0) {
			rotate = int(length) + int(rotation);
		}

		if (int(rotate) > 0 && int(length) > 1) addCode(greater);
		else if (int(length) > 1) addCode(other);
		else addCode(one);
	}
	exectime {
		int cost = 0;
		if (int(length) > 1 && int(rotation) == 0)
			cost = 4 + 2*int(length);
		else if (length == 1)
			cost = 3;
		else
			cost = 7 + 4 * int(length);
		return cost;
	}
}

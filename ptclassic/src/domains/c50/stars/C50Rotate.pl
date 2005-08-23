defstar {
	name { Rotate }
	domain { C50 }
	desc { Rotate a block of input data }
	version {@(#)C50Rotate.pl	1.9	05/26/98}
	author { A. Baensch, Luis Gutierrez, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
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

	protected{
	// effective length of the data(to allow for complex inputs)
		int effLength;
	// effective rotation
		int effRotation;
	// start address for block move
		int start;
	}

	codeblock(greater,"") {
	lacc	#$addr(input)
	samm	cbsr1
	add	#@(effLength-1)
	samm	cber1
	lacl	#9
	samm	cbcr
	mar	*,ar1
	lar	ar1,#($addr(input)+@start)
	rpt	#@(effLength-1)
	bldd	*+,#$addr(output)	
       	}

	codeblock(other,"") {
	mar    	*,AR0       			;
 	lar     AR0,#$addr(input)		;Address input		=> AR0
	rpt     #@(effLength-1)			;for number of length
	bldd	*+,#$addr(output)		;output(i) = input(i)
	}

	codeblock(one) {
	lmmr	ar1,#$addr(input)
        nop
        nop
	smmr	ar1,#$addr(output)
	} 

	setup {
		if (input.resolvedType() == COMPLEX) {
			effLength = 2*int(length);
			effRotation = 2*int(rotation);
		}else{ 
			effLength = int(length);
			effRotation = int(rotation);
		}
		input.setSDFParams(int(length),int(length)-1);
		output.setSDFParams(int(length),int(length)-1);
	}
	go {
		int i = effRotation;

		if (i > int(effLength) || -i > int(effLength)) {
		    Error::abortRun(*this,
				    "Number of rotations > block length");
		    return;
		}

		start = effRotation;
		if (int(effRotation) < 0) {
			start = int(effLength) + int(effRotation);
		}

		if (int(start) > 0 && int(effLength) > 1) addCode(greater());
		else if (int(effLength) > 1) addCode(other());
		else addCode(one);
	}
	exectime {
		int cost = 0;
		if (int(effLength) > 1 && int(effRotation) == 0)
			cost = 4 + 2*int(effLength);
		else if (effLength == 1)
			cost = 3;
		else
			cost = 9 +  int(effLength);
		return cost;
	}
}

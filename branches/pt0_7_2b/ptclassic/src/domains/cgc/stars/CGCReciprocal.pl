defstar {
	name {Reciprocal}
	domain { CGC }
	desc {  1/x, with an optional magnitude limit. }
	version { @(#)CGCReciprocal.pl	1.10	10/07/96 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC nonlinear library }
	htmldoc {
This star computes 1<i>/x</i>, where <i>x</i> is the input.
If the <i>magLimit</i> parameter is not 0.0, then the output is
&#177;max(<i>magLimit, </i>1<i>/x </i>).  In this case, <i>x</i> can be zero
without causing an floating exception.
The sign of the output is determined by the sign of the input.
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	defstate {
		name{magLimit}
		type{float}
		default{"0.0"}
		desc {If non-zero, limits the magnitude of the output.}
	}
	constructor {
		// Indicate that there is no dynamically changing internal
		// state, so the star can be parallelized.
		noInternalState();
	}
	go {
	    if(double(magLimit) == 0.0)
		addCode(reciprocal);
	    else
		addCode(satrec);
	}
   codeblock(reciprocal) {
	$ref(output) = 1/$ref(input);
   }
   codeblock(satrec) {
	if($ref(input) == 0.0)
	    $ref(output) = $val(magLimit);
	else {
	    double t;
	    t = 1/$ref(input);
	    if (t>$val(magLimit))
		$ref(output) = $val(magLimit);
	    else if (t < - $val(magLimit))
		$ref(output) = - $val(magLimit);
	    else
		$ref(output) = t;
	}
   }
	exectime {
		/* based on CG96Reciprocal */
		if (double(magLimit) == 0.0) return 8;
		else return 12;
	}
}

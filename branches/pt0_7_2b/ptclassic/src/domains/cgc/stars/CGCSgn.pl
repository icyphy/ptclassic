defstar {
	name {Sgn}
	domain {CGC}
	desc {
This star computes the signum of its input.
The output is +/- 1.
Note that 0.0 maps to 1.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
.Id "signum"
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{int}
	}
	codeblock(std) {
	  double x;
	  int sgn;
	  x = $ref(input);
	  sgn = (x >= 0.0) ? 1 : -1;
	  $ref(output) = sgn;
	}
	go {
	  addCode(std);
	}
}

defstar {
	name {Limit}
	domain {SDF}
	desc { Hard limiter. }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
This star hard limits input samples to keep the in the range
of (\fIbottom, top\fR).
.Id "hard limiter"
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
		name{bottom}
		type{float}
		default{"0.0"}
		desc {Lower limit of the output.}
	}
	defstate {
		name{top}
		type{float}
		default{"1.0"}
		desc {Upper limit of the output.}
	}
	go {
	   if(double(input%0) > double(top))
		output%0 << double(top);
	   else if (double(input%0) < double(bottom))
		output%0 << double(bottom);
	   else
		output%0 = input%0;
	}
}

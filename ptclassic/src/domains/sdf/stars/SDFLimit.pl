defstar {
	name {Limit}
	domain {SDF}
	desc { 
The output of this star is the value of the input limited
to the range between "bottom" and "top" inclusive.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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

defstar {
	name { Exp }
	domain { SDF } 
	desc { 	Compute the exponential function of the input. }
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<b>Bugs:</i></b> Overflow is not handled well.
<a name="exponential"></a>
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	ccinclude { <math.h> }
	go {
		output%0 << exp(double(input%0));
	}
}

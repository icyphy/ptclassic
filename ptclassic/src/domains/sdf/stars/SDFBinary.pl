defstar {
	name {Binary}
	domain {SDF}
	desc {
Binary threshold of the input.  If the input is greater than zero,
then a 1 is output; otherwise, the output is 0 if outputBipolarValue
is False or -1 otherwise.
	}
	author { Biao Lu }
	acknowledge { Brian L. Evans }
	version { $Revision$	$Date$ }
	location { SDF main library }
	copyright {
Copyright (c) 1997 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	htmldoc {
Binary and bipolar are two kinds of the activation functions.
The output of the binary is either 1 or 0, and the output of the bipolar
is either 1 or -1.
	}
	input {
		name{ input }
		type{ float }
	}
	output {
		name { output }
		type { float }
	}
	private {
		double falseValue;
	}
	state {
	    name { outputBipolarValue }
	    type { int }
	    default { NO }
	    desc {
If true, output -1 if the input is negative and 1 otherwise.
If false, output 0 if the input is negative and 1 otherwise.
	    }
	}
	setup {
	    falseValue = ( int(outputBipolarValue) ? -1.0 : 0.0 );
	}
	go {
	   if (double(input%0) > 0.0)
		output%0 << 1.0;
	   else 
		output%0 << falseValue;
       }
}

defstar {
	name { Select }
	domain { BDF }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	author { J. T. Buck }
	location { BDF main library }
	desc {
If the value on the 'control' line is nonzero, trueInput
is copied to the output; otherwise, falseInput is.
	}
	input {
		name { trueInput }
		type { ANYTYPE }
	}
	input {
		name { falseInput }
		type { =trueInput }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =trueInput }
	}
	constructor {
		trueInput.setBDFParams(1,control,BDF_TRUE);
		falseInput.setBDFParams(1,control,BDF_FALSE);
	}
	go {
	// get control Particles from Geodesic
		control.receiveData();	
		
		// read control value, and route input to output
		// depending on it.
		if ((int) (control%0)) {
			// route true-side to the output
			trueInput.receiveData();
			output%0 = trueInput%0;
		} else {
			// route false-side to the output
			falseInput.receiveData();
			output%0 = falseInput%0;
		}
		output.sendData();
	}
}



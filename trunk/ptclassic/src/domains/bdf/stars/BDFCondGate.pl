defstar {
	name { True }
	domain { BDF }
	version { $Id$ }
	author { J. T. Buck }
	location { BDF main library }
	desc {
If the value on the 'control' line is nonzero, input is copied to output;
otherwise, the control token is just eaten.  This is effectively one half
of a Select gate.
	}
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =input }
	}
	constructor {
		input.setBDFParams(1,control,BDF_TRUE);
		output.setBDFParams(1,control,BDF_TRUE, 0);
	}
	go {
		control.grabData();
		if (int(control%0)) {
			input.grabData();
			output%0 = input%0;
			output.sendData();
		}
	}
}

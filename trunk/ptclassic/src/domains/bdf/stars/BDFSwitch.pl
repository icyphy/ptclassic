defstar {
	name { Switch }
	domain { BDF }
	version { $Id$ }
	author { J. T. Buck }
	location { BDF main library }
	desc {
Switches input events to one of two outputs, depending on
the value of the control input.  If control is true, the
value is written to trueOutput; otherwise it is written to
falseOutput.
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
		name { trueOutput }
		type { =input }
	}
	output {
		name { falseOutput }
		type { =input }
	}
	constructor {
		trueOutput.setBDFParams(1, control, BDF_TRUE, 0);
		falseOutput.setBDFParams(1, control, BDF_FALSE, 0);
	}
	go {
		// get Particles from Geodesic
		control.grabData();
		input.grabData();

		// read control value, and route input
		// to output depending on it.
		if ((int) (control%0)) {
			trueOutput%0 = input%0;
			trueOutput.sendData();
		} else {
			falseOutput%0 = input%0;
			falseOutput.sendData();
		}
	}
}



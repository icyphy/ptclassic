defstar {
	name {Not}
	domain {BDF}
	desc { A logical inverter: output is NOT of input }
	version { $Id$ }
	location { BDF main library }
	explanation {
This star is a logical inverter; it converts TRUE (non-zero) values on
input to FALSE (zero) values on output and vice versa.  Furthermore,
the scheduler can use this information to deduce properties of boolean
signals.
	}
	input {
		name{input}
		type{int}
	}
	output {
		name{output}
		type{int}
	}
	constructor {
		output.setBDFParams (1, input, BDF_COMPLEMENT);
	}
	go {
		// note that explicit grabData and sendData are needed
		input.grabData();
		output%0 << !int(input%0);
		output.sendData();
	}
}


defstar {
	name { Case }
	domain { DDF }
	desc {
Route an input particle to one of the outputs depending on the 
control particle.  The control particle should have value between
zero and N-1, inclusive, where N is the number of outputs.  It
can be used with EndCase to implement an if ... then construct.
	}
	version { @(#)DDFCase.pl	1.14	06 Oct 1996 }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DDF library }
	seealso { EndCase, $PTOLEMY/src/domains/hof/stars/HOFIfElse.htm,
		    $PTOLEMY/src/domains/hof/stars/HOFIfElseGr.htm }
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
		desc { selects the output port to receive the input value. }
	}
	outmulti {
		name { output }
		type { =input }
		num { 0 }
	}
	go {
		// get Particles from Geodesic
		control.receiveData();
		input.receiveData();

		// test the current value of the control signal
		int outputNum = int(control%0);
		if ( outputNum < 0 ) {
		    Error::abortRun(*this, "control value is negative");
		    return;
		}
		else if ( outputNum >= output.numberPorts() ) {
		    Error::abortRun(*this, "control value too large");
		    return;
		}

		// read control value, and route input
		// to output depending on it.
		MPHIter nexti(output);
		for (int i = 0; i < outputNum; i++) {
		    nexti++;
		}
		OutDDFPort& oport = *(OutDDFPort *)nexti++;
		oport%0 = input%0;
		oport.sendData();
	}
}

defstar {
	name {Filter}
	domain {DE}
	version { $Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
This star filters the input signal with a first-order, AR impulse response.
The data input is interpreted as weighted impulses (Dirac delta functions).
An output is triggered by a clock input.
	}
	htmldoc {
Assuming a clock input event
triggers the star at time <i>T</i>, the output is:
<pre>
y(T) ~=~ d sup {(T ~-~ tau )} s( tau )
</pre>
where <i>tau</i> is the time of the latest data input event, <i>s</i>(<i> tau </i>)<i></i> is the
state of the filter at time <i>tau</i>,
and <i>d</i> is the exponential decay parameter.
Each time a data input arrives, the state is updated
as follows:
<pre>
s( tau ) ~=~ i( tau ) ~+~ d sup {( tau ~-~ tau prime )} s( tau prime )
</pre>
where <i>tau prime</i> is the time of the previous data input event,
and <i>i</i>(<i> tau </i>)<i></i> is the data input.
	}
	input {
		name {clock}
		type {anytype}
	}
	input {
		name {data}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	defstate {
		name {decay}
		type {float}
		default {"0.9"}
		desc { Exponential decay parameter. }
	}
	defstate {
		name {tau}
		type {float}
		default {"0.0"}
		desc { Time of the latest data input event. }
	}
	defstate {
		name {state}
		type {float}
		default {"0.0"}
		desc { State of the filter as of the latest input event. }
	}
	constructor {
		// data input does not produce an immediate output
		data.triggers();
		data.before(clock);
	}
	go {
	   // If the data input is new, update the state.
	   if (data.dataNew) {
		state = double(data.get()) + double(state) *
		   pow(double(decay),(arrivalTime-double(tau)));
		tau = arrivalTime;
	   }
	   // Produce an output only if the clock input is new.
	   if (clock.dataNew) {
		double t = double(state) *
			pow(double(decay),(arrivalTime-double(tau)));
		completionTime = arrivalTime;
		output.put(completionTime) << t;
		clock.dataNew = FALSE;
	   }
	}
}

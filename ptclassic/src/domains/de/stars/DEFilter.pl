ident {
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  E. A. Lee
Date of creation: 10/9/90

This star filters the input signal with a first-order, AR impulse response.
The data input is interpreted as weighted impulses.  An output is triggered
by a clock input.  Assuming a clock input event triggers the star at time $T$,
the output is:
.EQ
y(T) ~=~ d sup {(T ~-~ tau)} s( tau )
.EN
where $tau$ is the time of the latest data input event, $s( tau )$ is the
state of the filter at time $tau$,
and $d$ is the exponential decay parameter.
Each time a data input arrives, the state is updated
as follows:
.EQ
s( tau ) ~=~ i( tau ) ~+~ d sup {( tau ~-~ tau prime )} s( tau prime )
.EN
where $tau prime$ is the time of the previous data input event,
and $i( tau )$ is the data input.
**************************************************************************/
}
defstar {
	name {Filter}
	domain {DE}
	desc {
	   "First order AR filter."
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
		desc { "Exponential decay parameter." }
	}
	defstate {
		name {tau}
		type {float}
		default {"0.0"}
		desc { "Time of the latest data input event." }
	}
	defstate {
		name {state}
		type {float}
		default {"0.0"}
		desc { "State of the filter as of the latest input event." }
	}

	go {
	   // If the data input is new, update the state.
	   if (data.dataNew) {
		state = double(data%0) + double(state) *
		   pow(double(decay),(arrivalTime-double(tau)));
		tau = arrivalTime;
		data.dataNew = FALSE;
	   }
	   // Produce an output only if the clock input is new.
	   if (clock.dataNew) {
		double t = double(state) *
			pow(double(decay),(arrivalTime-double(tau)));
		completionTime = arrivalTime;
		output.put(completionTime) << float(t);
		clock.dataNew = FALSE;
	   }
	}
}

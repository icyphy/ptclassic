defstar {
	name		{ PseudoCell }
	domain		{ DE }
	version		{ $Id$ }
	author		{ Paul Haskell }
	copyright	{ 1992 The Regents of the University of California }
	location	{ DE main library }
	desc {
Every time this star receives an input, it outputs an empty NetworkCell.
Thus, this star's output can be used to load a simulated network.
	}

	input {		name { input }	type { anytype } }
	output {	name { output }	type { message } }

	defstate {
		name	{ Priority }
		type	{ int }
		default	{ 0 }
		desc	{ Priority level for outgoing cells. }
	}
	defstate {
		name	{ destination }
		type	{ int }
		default	{ 0 }
		desc	{ Destination addresses. }
	}

	hinclude { "NetworkCell.h" }

	go {
		completionTime = arrivalTime;

		LOG_NEW; NetworkCell* cell = new NetworkCell( int(Priority),
				int(destination));
		Envelope outEnvp(*cell);
		output.put(completionTime) << outEnvp;
	}
}

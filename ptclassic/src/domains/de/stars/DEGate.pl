defstar {
	name { Gate }
	domain { DE }
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
Initially, all "input" particles are passed to the output (gate is open).
When a TRUE (non-zero) is received on the "control" input, the gate is closed.
A FALSE (zero) particle on the "control" input re-opens the gate.
While the gate is closed, "input" particles are either discarded
(if "discard" = TRUE) or ignored (if "discard" = FALSE).
Ignoring the inputs has the effect of queueing them on the input geodesic.
	}
	explanation {
When \fIdiscard\fR is FALSE, and the gate is closed for a while,
its behavior when re-opened is a little subtle.  Upon re-opening,
an output is immediately produced, if any particle was arrived
during the time the gate was closed.  The time stamp of the output
will be that of the \fIcontrol\fR particle that re-opened the gate.
Subsequently,
each time it receives an \fIinput\fR, it will produce an output with the
same time stamp as the input.  But if more than one particle was received
while the gate was closed, then
the value of the output will be that of some past sample.
In particular, the second output after re-opening the gate has
the value of the second input received while the gate was closed,
and occurs at the time of the first input received after the gate
is re-opened.
.pp
Note that if the predecessor star is a
.c Queue,
then it is easy to prevent the arrival of more than one input
while the gate is closed.
This is demonstrated in the
\fIblockage\fR demo.
	}
	seealso { blockage Discard }
	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
		desc { Controls whether gate is open or closed. }
	}
	output {
		name { output }
		type { ANYTYPE }
	}
	defstate {
		name { discard }
		type { int }
		default { TRUE }
		desc { Determines what happens to inputs when gate is closed. }
	}
	private {
		int blocked;
	}
	constructor {
		input.inheritTypeFrom(output);
	}
	start {
		blocked = FALSE;
	}

	go {
		// get input
		completionTime = arrivalTime;

		// accept blockRequest or blockRelease input
		if (control.dataNew) {
			if (int(control.get()) == TRUE) {
				blocked = TRUE;
			} else {
				blocked = FALSE;
			}
		}	

		// receive the normal input
		if (input.dataNew) {
		    if (blocked == FALSE) { // process the input
			output.put(completionTime) = input.get();
		    } else
			if (discard == TRUE)
			    input.get();
		}
	}
}


defstar {
	name {DB}
	domain {CGC}
	desc {
Converts input to dB.  Zero and negative values are
converted to "min" (default -100).
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	explanation {
For inputs that are greater than zero, the output either
$N log sub 10 (input)$ or \fImin\fR, whichever is larger, where
$N ~=~ 10$ if \fIinputIsPower\fR is TRUE, and $N ~=~ 20$ otherwise.
.Id "decibel"
The default is $N ~=~ 20$.
For inputs that are zero or negative, the output is \fImin\fR.
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	state {
		name{min}
		type{float}
		default{"-100"}
		desc{Minimum output value.}
	}
	state {
		name{inputIsPower}
		type{int}
		default{"FALSE"}
		desc{
TRUE if input is a power measurement, FALSE if it's an amplitude measurement.
		}
	}
	state {
		name{gain}
		type{float}
		default{"20.0"}
		attributes { A_NONSETTABLE }
	}
	constructor {
		noInternalState();
	}
	start {
		if (int(inputIsPower)) gain=10.0;
		addInclude("<math.h>");
	}
	codeblock(body) {
	double f = $ref(input);
	if (f <= 0.0) $ref(output) = $val(min);
	else {
		f = $val(gain) * log10 (f);
		if (f < $val(min)) f = $val(min);
	}
	}
	go {
		addCode(body);
	}
}

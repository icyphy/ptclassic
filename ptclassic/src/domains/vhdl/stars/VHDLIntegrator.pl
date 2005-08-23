defstar {
	name { Integrator }
	domain { VHDL }
	desc {
An integrator with leakage, limits, and reset.
With the default parameters, input samples are simply accumulated,
and the running sum is the output.  To prevent any resetting in the
middle of a run, connect a d.c. source with value 0.0 to the "reset"
input.  Otherwise, whenever a non-zero is received on this input,
the accumulated sum is reset to the current input (i.e. no feedback).

Limits are controlled by the "top" and "bottom" parameters.
If top <= bottom, no limiting is performed (default).  Otherwise,
the output is kept between "bottom" and "top".  If "saturate" = YES,
saturation is performed.  If "saturate" = NO, wrap-around is performed
(default).  Limiting is performed before output.

Leakage is controlled by the "feedbackGain" state (default 1.0).
The output is the data input plus feedbackGain*state, where state
is the previous output.
	}
	version { @(#)VHDLIntegrator.pl	1.3 10/01/96 }
	author { Michael C. Williamson, E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	htmldoc {
<a name="filter, integrator"></a>
	}
	input {
		name { data }
		type { float }
	}
	input {
		name { reset }
		type { int }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name {feedbackGain}
		type {float}
		default {"1.0"}
		desc { The gain on the feedback path.}
	}
	defstate {
		name {top}
		type {float}
		default {"0.0"}
		desc { The upper limit.}
	}
	defstate {
		name {bottom}
		type {float}
		default {"0.0"}
		desc { The lower limit.}
	}
	defstate {
		name {saturate}
		type {int}
		default {"YES"}
		desc { Saturate if YES, wrap around otherwise.}
	}
	defstate {
		name {state}
		type {float}
		default {"0.0"}
		desc { An internal state.}
		attributes { A_NONCONSTANT | A_SETTABLE }
	}
	protected {
		double spread;
	}
	setup {
		spread = double(top) - double(bottom);
	}
	codeblock (integrate) {
if ($ref(reset) /= 0)
  $temp(result,float) $assign(result,float) $ref(data);
else
  $temp(result,float) $assign(result,float) $ref(data) + $val(feedbackGain) * $ref(state);
end if;
	}
	codeblock (limitWithSat) {
-- Limiting is in effect
-- Take care of the top
if ($temp(result,float) > $val(top)) $temp(write,float) = $val(top);
-- Take care of the bottom
if ($temp(result,float) < $val(bottom)) $temp(write,float) = $val(bottom);
	}
	codeblock (limitWithoutSat) {
-- Limiting is in effect
-- Take care of the top
while ($temp(write,float) > $val(top)) loop
  $temp(write,float) := $temp(write,float) - ($val(top) - $val(bottom));
end loop;
-- Take care of the bottom
while ($temp(write,float) < $val(bottom)) loop
  $temp(write,float) := $temp(write,float) + ($val(top) - $val(bottom));
end loop;
        }
	codeblock (write) {
	    $ref(state) $assign(state) $temp(write,float);
	    $ref(output) $assign(output) $temp(write,float);
	}
	go {
	    addCode(integrate);
	    if (spread > 0.0) 
		if (int(saturate))
		    addCode(limitWithSat);
		else
		    addCode(limitWithoutSat);
	    addCode(write);
	}
}

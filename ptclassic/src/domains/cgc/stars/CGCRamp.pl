defstar {
	name { FloatRamp }
	domain { CGC }
	desc {
Generates a ramp signal, starting at "value" (default 0)
with step size "step" (default 1).
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1992 The Regents of the University of California }
	location { CGC main library }
	output {
		name { output }
		type { float }
	}
	defstate {
		name { step }
		type { float }
		default { 1.0 }
		desc { Increment from one sample to the next. }
	}
	defstate {
		name { value }
		type { float }
		default { 0.0 }
		desc { Initial (or latest) value output by Ramp. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	go { gencode(std); }
	codeblock (std) {
	$ref(output) = $ref(value);
	$ref(value) += $val(step);
	}
}

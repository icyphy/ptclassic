defstar {
	name { Trainer }
	domain { CGC }
	desc {
Passes the "train" input to the output for the first "trainLength"
samples, then passes the "decision" input to the output.  Designed
for use in decision feedback equalizers, but can be used for other
purposes.
	}
	version { $Id$ }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
.Id "DFE training"
.Id "decision feedback equalizer training"
.Id "equalizer, decision feedback, training"
	}
	defstate {
		name { trainLength }
		type { int }
		default { 100 }
		desc { Number of training samples to use }
	}
	input {
		name { train }
		type { anytype }
	}
	input {
		name { decision }
		type { =train }
	}
	output {
		name { output }
		type { = train }
	}
	state {
		name { count }
		type { int }
		default { "0" }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
		desc { local variable for counting inputs }
	}
	constructor {
		noInternalState();
	}

	setup {
		count = 0;
	}

	begin {
		// handle precisions for fix types
		if (train.resolvedType() == FIX) {

			// if the precision for the output port is not defined
			// by the successor star, the precisions of the input
			// ports are passed through to the output ports

			if (!output.precision().isValid())
				output.setAttributes(A_VARPREC);
		}
	}

	go {
		// check for fix types
		if (train.resolvedType() == FIX) {

			// if we use variable precision representation,
			// set the precision of the output port from
			// the source
			if (output.attributes() & AB_VARPREC) {

@	if ($ref(count) < $val(trainLength)) {
@		$precision(output).len  = FIX_GetLength($ref(train));
@		$precision(output).intb = FIX_GetIntBits($ref(train));
@		FIX_Assign($ref(output),$ref(train));
@		$ref(count)++;
@	} else {
@		$precision(output).len  = FIX_GetLength($ref(decision));
@		$precision(output).intb = FIX_GetIntBits($ref(decision));
@		FIX_Assign($ref(output),$ref(decision));
@	}

			} else {

@	if ($ref(count) < $val(trainLength)) {
@		FIX_Assign($ref(output),$ref(train));
@		$ref(count)++;
@	} else {
@		FIX_Assign($ref(output),$ref(decision));
@	}

			}

		} else {

@	if ($ref(count) < $val(trainLength)) {
@		$ref(output) = $ref(train);
@		$ref(count)++;
@	} else {
@		$ref(output) = $ref(decision);
@	}

		}
	}
	exectime {
		return 1;
	}
}

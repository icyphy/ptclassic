defstar {
	name {Neuron}
	domain {SDF}
	desc {
This neuron will output the sum of the weighted inputs, as a floating value.
	}
	author {Biao Lu}
	acknowledge { Brian L. Evans }
       	location { SDF main library }
	version{ $Revision$	$Date$ }
	copyright {
Copyright (c) 1997 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	inmulti {
		name { input }
		type { float }
		desc {
The neuron can have the multiple inputs with the different weights.
		}
	}
	output {
		name { output }
		type { float }
		desc {
The output will be only one which is the sum of the weighted inputs.
		}
	}
	state {
	    name { weights }
	    type {floatarray}
	    default {"1.0 1.0 1.0 1.0"}
	    desc {
Weights are associated with the connections between the two layers.
They can be input by the users. All the weights must be precalculated.
The number of the weights must be equal to the number of the inputs.
	    }
	}
	state {
	    name {number}
	    type {int}
	    default { "10" }
	    desc { This number control the array size. }
	}
	setup {
		if ( input.numberPorts() != weights.size() ) {
		    StringList msg = "The number of inputs ";
		    msg << input.numberPorts()
			<< " do not equal the number of weights "
			<< weights.size();
		    Error::abortRun(*this, msg);
		    return;
		}
	}
	go {
		MPHIter nexti(input);
		PortHole *p;
		double out = 0.0;
		int k = 0;
		while ((p = nexti++) != 0 ) {
		    double inputValue = double((*p)%0);
		    out += weights[k] * inputValue;
		    k++;
		}
		output%0 << out;
	}
}

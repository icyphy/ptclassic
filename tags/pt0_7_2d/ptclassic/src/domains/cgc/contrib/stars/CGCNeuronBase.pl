defstar {
        name { NeuronBase }
        domain {CGC}
        desc {
This is a McCulloch-Pitts neuron. The activation of this neuron is binary.
That is, at any time step, the neuron either fires, or does not fires.
        }
        author {Biao Lu}
        acknowledge { Brian L. Evans }
        location { CGC contrib library }
	version { @(#)CGCNeuronBase.pl	1.2	03/24/98 }
        copyright {
Copyright (c) 1997-1998 The University of Texas.
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
	constructor {
	  noInternalState();
	}
        setup {
	    if ( input.numberPorts() != weights.size() ) {
		StringList msg = "The number of inputs ";
		msg << input.numberPorts()
		    << " does not equal the number of weights "
			<< weights.size();
	      Error::abortRun(*this, msg);
		return;
	  }
        }

	codeblock(initial) {
	  double kthweight, out = 0.0;
	  int index = 0;
	}

	codeblock(iteration, "int k") {
	  kthweight = $ref2(weights,index);
	  out += kthweight * $ref(input#@k);
	  index++;
	}
	go {
	  addCode(initial);
	  // Must begin at 0 since we will index the weights array
	  // Do not begin with 0 for the input. It is not a port nor a state
	  for (int k = 1; k <= input.numberPorts(); k++) {
	    addCode(iteration(k));
	  }
        }
        exectime {
            return input.numberPorts();
        }
}

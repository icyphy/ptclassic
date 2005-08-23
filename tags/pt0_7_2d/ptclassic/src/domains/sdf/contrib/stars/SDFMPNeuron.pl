defstar {
	name {MPNeuron}
	domain {SDF}
	desc {
This is a McCulloch-Pitts neuron derived from NeuronBase. The activation 
of this neuron is binary. That is, at any time step, the neuron either fires, 
or does not fires.
	}
	author {Biao Lu}
	acknowledge { Brian L. Evans }
       	location { SDF contrib library }
	version { @(#)SDFMPNeuron.pl	1.1	03/24/98 }
	copyright {
Copyright (c) 1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
        htmldoc {
This star is based on the McCulloch-Pitts model of artificial neurons [1].
<p>
REFERENCES
<ol>
<li>W. S. McCulloch and W. Pitts, "A logical calculus of the ideas immanent 
in nervous activity", <em>Bulletin of Mathematical Biophysics</em>, Vol. 5, 
pp. 115-133, 1943
</ol>
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
	    name { threshold }
	    type { float }
	    default { 1.0 }
	    desc {
This is the threshold for each neuron. If the sum of the weighted signal
is greater than or equal to this threshold, the neuron will be fired.
	    }
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
		if (out >= double(threshold))
		    out = 1.0;
		else
		    out = 0.0;

		output%0 << out;
	}
}

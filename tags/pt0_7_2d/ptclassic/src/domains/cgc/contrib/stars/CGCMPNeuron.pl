defstar {
        name { MPNeuron }
        domain {CGC}
	derivedFrom { NeuronBase }
        desc {
This is a McCulloch-Pitts neuron. The activation of this neuron is binary.
That is, at any time step, the neuron either fires, or does not fires.
        }
        author {Biao Lu}
        acknowledge { Brian L. Evans }
        location { CGC contrib library }
        version { @(#)CGCMPNeuron.pl	1.1	03/24/98 }
        copyright {
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        htmldoc {
This MPNeuron is based on the McCulloch-Pitts model of artificial neurons [1].
<p> 
REFERENCES
<ol>
<li>W. S. McCulloch and W. Pitts, <em>"A logical calculus of the ideas immanent
in nervous activity"</em>, Bulletin of Mathematical Biophysics, Vol. 5,
pp. 115-133, 1943
</ol>
        }
	defstate {
	    name { threshold }
	    type { float }
	    default { "1.0" }
	    desc {
This is the threshold for each neuron. If the sum of the weighted signal
is greater than or equal to this threshold, the neuron will be fired.
                 }
	}
	codeblock(final) {
	  if ( out >= $val(threshold))
	    out = 1.0;
	  else
	    out = 0.0;

	  $ref(output) = out;
	}

	go {
	  CGCNeuronBase::go();
	    addCode(final);
        }
	exectime {
	    return input.numberPorts();
	}	
}

defstar {
        name { Neuron }
        domain {CGC}
	derivedFrom { NeuronBase }
        desc {
This is a McCulloch-Pitts neuron. The activation of this neuron is binary.
That is, at any time step, the neuron either fires, or does not fires.
        }
        author {Biao Lu}
        acknowledge { Brian L. Evans }
        location { CGC contrib library }
	version { @(#)CGCNeuron.pl	1.2	03/24/98 }
        copyright {
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	codeblock(final) {
	  $ref(output) = out;
	}
	go {
	    CGCNeuronBase::go();
	      addCode(final);
        }
}

static const char file_id[] = "CGCNeuronBase.pl";
// .cc file generated from CGCNeuronBase.pl by ptlang
/*
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCNeuronBase.h"

const char *star_nm_CGCNeuronBase = "CGCNeuronBase";

const char* CGCNeuronBase :: className() const {return star_nm_CGCNeuronBase;}

ISA_FUNC(CGCNeuronBase,CGCStar);

Block* CGCNeuronBase :: makeNew() const { LOG_NEW; return new CGCNeuronBase;}

CodeBlock CGCNeuronBase :: initial (
# line 57 "CGCNeuronBase.pl"
"	  double kthweight, out = 0.0;\n"
"	  int index = 0;\n");

const char* CGCNeuronBase :: iteration(int k) {
# line 62 "CGCNeuronBase.pl"
	static StringList _str_; _str_.initialize(); _str_ << 
"	  kthweight = $ref2(weights,index);\n"
"	  out += kthweight * $ref(input#" << (k) << ");\n"
"	  index++;\n";
	return (const char*)_str_;
}

CGCNeuronBase::CGCNeuronBase ()
{
	setDescriptor("This is a McCulloch-Pitts neuron. The activation of this neuron is binary.\nThat is, at any time step, the neuron either fires, or does not fires.");
	addPort(input.setPort("input",this,FLOAT));
	addPort(output.setPort("output",this,FLOAT));
	addState(weights.setState("weights",this,"1.0 1.0 1.0 1.0","Weights are associated with the connections between the two layers.\nThey can be input by the users. All the weights must be precalculated.\nThe number of the weights must be equal to the number of the inputs."));

# line 44 "CGCNeuronBase.pl"
noInternalState();
}

int CGCNeuronBase::myExecTime() {
# line 76 "CGCNeuronBase.pl"
return input.numberPorts();
}

void CGCNeuronBase::setup() {
# line 47 "CGCNeuronBase.pl"
if ( input.numberPorts() != weights.size() ) {
		StringList msg = "The number of inputs ";
		msg << input.numberPorts()
		    << " does not equal the number of weights "
			<< weights.size();
	      Error::abortRun(*this, msg);
		return;
	  }
}

void CGCNeuronBase::go() {
# line 68 "CGCNeuronBase.pl"
addCode(initial);
	  // Must begin at 0 since we will index the weights array
	  // Do not begin with 0 for the input. It is not a port nor a state
	  for (int k = 1; k <= input.numberPorts(); k++) {
	    addCode(iteration(k));
	  }
}

// prototype instance for known block list
static CGCNeuronBase proto;
static RegisterBlock registerBlock(proto,"NeuronBase");

static const char file_id[] = "CGCMPNeuron.pl";
// .cc file generated from CGCMPNeuron.pl by ptlang
/*
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCMPNeuron.h"

const char *star_nm_CGCMPNeuron = "CGCMPNeuron";

const char* CGCMPNeuron :: className() const {return star_nm_CGCMPNeuron;}

ISA_FUNC(CGCMPNeuron,CGCNeuronBase);

Block* CGCMPNeuron :: makeNew() const { LOG_NEW; return new CGCMPNeuron;}

CodeBlock CGCMPNeuron :: final (
# line 38 "CGCMPNeuron.pl"
"	  if ( out >= $val(threshold))\n"
"	    out = 1.0;\n"
"	  else\n"
"	    out = 0.0;\n"
"\n"
"	  $ref(output) = out;\n");

CGCMPNeuron::CGCMPNeuron ()
{
	setDescriptor("This is a McCulloch-Pitts neuron. The activation of this neuron is binary.\nThat is, at any time step, the neuron either fires, or does not fires.");
	addState(threshold.setState("threshold",this,"1.0","This is the threshold for each neuron. If the sum of the weighted signal\nis greater than or equal to this threshold, the neuron will be fired."));


}

int CGCMPNeuron::myExecTime() {
# line 52 "CGCMPNeuron.pl"
return input.numberPorts();
}

void CGCMPNeuron::go() {
# line 48 "CGCMPNeuron.pl"
CGCNeuronBase::go();
	    addCode(final);
}

// prototype instance for known block list
static CGCMPNeuron proto;
static RegisterBlock registerBlock(proto,"MPNeuron");

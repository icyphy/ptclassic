static const char file_id[] = "CGCNeuron.pl";
// .cc file generated from CGCNeuron.pl by ptlang
/*
Copyright (c) 1997-1998 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCNeuron.h"

const char *star_nm_CGCNeuron = "CGCNeuron";

const char* CGCNeuron :: className() const {return star_nm_CGCNeuron;}

ISA_FUNC(CGCNeuron,CGCNeuronBase);

Block* CGCNeuron :: makeNew() const { LOG_NEW; return new CGCNeuron;}

CodeBlock CGCNeuron :: final (
# line 19 "CGCNeuron.pl"
"	  $ref(output) = out;\n");

CGCNeuron::CGCNeuron ()
{
	setDescriptor("This is a McCulloch-Pitts neuron. The activation of this neuron is binary.\nThat is, at any time step, the neuron either fires, or does not fires.");


}

void CGCNeuron::go() {
# line 23 "CGCNeuron.pl"
CGCNeuronBase::go();
	      addCode(final);
}

// prototype instance for known block list
static CGCNeuron proto;
static RegisterBlock registerBlock(proto,"Neuron");

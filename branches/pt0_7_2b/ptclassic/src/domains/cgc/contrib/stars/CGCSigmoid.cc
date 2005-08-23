static const char file_id[] = "CGCSigmoid.pl";
// .cc file generated from CGCSigmoid.pl by ptlang
/*
Copyright (c) 1997 The University of Texas.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCSigmoid.h"

const char *star_nm_CGCSigmoid = "CGCSigmoid";

const char* CGCSigmoid :: className() const {return star_nm_CGCSigmoid;}

ISA_FUNC(CGCSigmoid,CGCStar);

Block* CGCSigmoid :: makeNew() const { LOG_NEW; return new CGCSigmoid;}

CodeBlock CGCSigmoid :: exp (
# line 67 "CGCSigmoid.pl"
"        double rate = $val(learning_rate);\n"
"        $ref(output) = 1.0/(1.0 + exp(- rate * $ref(input)));\n");

CGCSigmoid::CGCSigmoid ()
{
	setDescriptor("Compute the Sigmoid function, defined as 1/(1 + exp(-r*input)), where\nr is the learning rate.  The Sigmoid function is commonly used in\nneural networks.");
	addPort(input.setPort("input",this,FLOAT));
	addPort(output.setPort("output",this,FLOAT));
	addState(learning_rate.setState("learning_rate",this,"0.5","The learning rate must be positive.  Once the network is well-trained,\nthe learning rate is fixed.  If you scale the weights of the neurons\nin a trained neural network, then you multiply the learning rate by the\nsame factor.  Scaling is important, for example, to guarantee that all\nof the intermediate computations remain in the range [-1,1)."));

# line 59 "CGCSigmoid.pl"
noInternalState();
}

int CGCSigmoid::myExecTime() {
# line 72 "CGCSigmoid.pl"
return 33;  /* use the same value as exp */
}

void CGCSigmoid::initCode() {
# line 62 "CGCSigmoid.pl"
addInclude("<math.h>");
}

void CGCSigmoid::go() {
# line 65 "CGCSigmoid.pl"
addCode(exp);
}

// prototype instance for known block list
static CGCSigmoid proto;
static RegisterBlock registerBlock(proto,"Sigmoid");

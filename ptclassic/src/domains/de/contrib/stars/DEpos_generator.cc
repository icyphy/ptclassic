static const char file_id[] = "DEpos_generator.pl";
// .cc file generated from DEpos_generator.pl by ptlang
/*
copyright (c) 1997 Dresden University of Technology,
                Mobile Communications Systems
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEpos_generator.h"
#include <ACG.h>

const char *star_nm_DEpos_generator = "DEpos_generator";

const char* DEpos_generator :: className() const {return star_nm_DEpos_generator;}

ISA_FUNC(DEpos_generator,DEStar);

Block* DEpos_generator :: makeNew() const { LOG_NEW; return new DEpos_generator;}

# line 49 "DEpos_generator.pl"
extern ACG* gen;

DEpos_generator::DEpos_generator ()
{
	setDescriptor("This star generates a uniformly distributed position in the area \n           (x_min, y_min) (x_max, y_max). The position is returned as a complex \n           value, where x is the real part and y is the imag. part of the \n           complex particle.");
	addPort(restart.setPort("restart",this,INT));
	addPort(R_Pos.setPort("R_Pos",this,COMPLEX));
	addState(x_min.setState("x_min",this,"300","x_min"));
	addState(x_max.setState("x_max",this,"5000","x_max"));
	addState(y_min.setState("y_min",this,"50","y_min"));
	addState(y_max.setState("y_max",this,"2600","y_max"));

# line 50 "DEpos_generator.pl"
random_x = NULL;
                  random_y = NULL;
}

DEpos_generator::~DEpos_generator() {
# line 53 "DEpos_generator.pl"
if (random_x) delete random_x;
                  if (random_y) delete random_y;
}

void DEpos_generator::setup() {
# line 57 "DEpos_generator.pl"
if (random_x) delete random_x;
        if (random_y) delete random_y;
	random_x = new Uniform(x_min, x_max, gen);
	random_y = new Uniform(y_min, y_max, gen);
}

void DEpos_generator::go() {
# line 63 "DEpos_generator.pl"
if (restart.dataNew) {
            double re = (*random_x) ();
            double im = (*random_y) ();
            R_Pos.put(arrivalTime) << Complex(re, im);
            restart.dataNew= FALSE;
        }
}

// prototype instance for known block list
static DEpos_generator proto;
static RegisterBlock registerBlock(proto,"pos_generator");

static const char file_id[] = "DEParallel2Serial.pl";
// .cc file generated from DEParallel2Serial.pl by ptlang
/*
copyright (c) 1997 Dresden University of Technology,
                    Mobile Communications Systems
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "DEParallel2Serial.h"

const char *star_nm_DEParallel2Serial = "DEParallel2Serial";

const char* DEParallel2Serial :: className() const {return star_nm_DEParallel2Serial;}

ISA_FUNC(DEParallel2Serial,DEStar);

Block* DEParallel2Serial :: makeNew() const { LOG_NEW; return new DEParallel2Serial;}

DEParallel2Serial::DEParallel2Serial ()
{
	setDescriptor("This is a parallel-to-serial converter. Data with the same time \n        stamp are read in parallel and put in a list. Each new value overrides \n        the older one in the list. Thus, the list always contains the very last\n        inputs. On demand the current contents of the list is given out serial.");
	addPort(x_in.setPort("x_in",this,FLOAT));
	addPort(y_in.setPort("y_in",this,FLOAT));
	addPort(demand.setPort("demand",this,ANYTYPE));
	addPort(x_out.setPort("x_out",this,FLOAT));
	addPort(y_out.setPort("y_out",this,FLOAT));

# line 45 "DEParallel2Serial.pl"
x_in.triggers();
	    y_in.triggers();
}

void DEParallel2Serial::setup() {
# line 49 "DEParallel2Serial.pl"
for (int o = 1; o <= 20; o++) {
		    listex[o] = 0.0;
		    listey[o] = 0.0;
		} 
		i = 0; j = 0;
}

void DEParallel2Serial::go() {
# line 56 "DEParallel2Serial.pl"
completionTime = arrivalTime;
	    while (x_in.dataNew) {
		i++;
		listex[i] = x_in.get();
		x_in.getSimulEvent();
	    }
	    while (y_in.dataNew) {
		j++;
		listey[j] = y_in.get();
		y_in.getSimulEvent();
	    } 
	    i = 0; j = 0;
	    if (demand.dataNew) {
		if (arrivalTime > 1.0) {
		    for (int o = 1; o <= 20; o++) {
			x_out.put(completionTime + i * 0.01) << listex[o];
			y_out.put(completionTime + i * 0.01) << listey[o];
		    }
		}
                // reset the list
		demand.dataNew = FALSE;
	    }
}

// prototype instance for known block list
static DEParallel2Serial proto;
static RegisterBlock registerBlock(proto,"Parallel2Serial");

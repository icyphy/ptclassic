defstar {
	name {LMSArrayStO}
	domain {SDF}
	desc {
An adaptive antenna array processor using the LMS algorithm.
The output signals of the antenna elements are taken
from a bus connected to the input.

The actual (time-dependent) steering vector is cyclically output, i.e.
on each invocation of the star you get one steering weight and after
length(steering) invocations you have one complete set of weights
which can be used e.g. for beam pattern calculation.
        }
	version {@(#)SDFLMSArrayStO.pl	1.0 9/24/96}
	author { U. Trautwein }
	copyright {
Copyright (c) 1996 Technical University of Ilmenau.
All rights reserved.
	}
	location { SDF main library }
	inmulti {
		name {input}
		type {complex}
	}
	input {
		name { error }
		type { complex }
	}
	defstate {
		name { stepSize }
		type { float }
		default { "0.01" }
		desc { Adaptation step size. }
	}
	defstate {
		name {steering}
		type {complexarray}
		default {
		"(1.0,0.0) (1.0,0.0) (1.0,0.0) (1.0,0.0)"
		}
		attributes { A_NONCONSTANT|A_SETTABLE }
		desc { Steering Weight Vector. }
	}
	defstate {
		name { errorDelay }
		type { int }
		default { "1" }
		desc {  Delay in the update loop. }
	}
	defstate {
		name { SaveSteeringFile }
		type { string }
		default { "" }
		desc { File to save final steering values. }
	}
	output {
		name {output}
		type {complex}
	}
	output {
	        name {StO}
		type {complex}
	}
	protected {
		int cyclecounter;
		int NumberElements;
		// store one set of steering values for output
		Complex *outputset;
	}
	constructor {
	    outputset = 0;
	}
	destructor {
	    LOG_DEL; delete [] outputset;
	}
	setup {
	        // We have to account for errorDelay past input snapshots
		input.setSDFParams(1, int(errorDelay));
		// delete previously allocated array
		LOG_DEL; delete [] outputset;
		// allocate memory
		NumberElements = steering.size();
		LOG_NEW; outputset = new Complex[NumberElements];
		for (int i = 0; i < NumberElements; i++)
		       outputset[i] = steering[i];
		cyclecounter = 0;
	}	
	go {
	        StO%0 << outputset[cyclecounter]; 
	        // Are we one cycle through with steering vector output?
		cyclecounter++;
		if (cyclecounter == NumberElements) {
		    cyclecounter = 0;
		    for (int i = 0; i < NumberElements; i++)
		       outputset[i] = steering[i];
		   }

		Complex e = Complex(error%0) * double(stepSize);
		int index = int(errorDelay);

		MPHIter nexti(input);
		PortHole *p;
		Complex sum = 0.0;
		int i = 0;
		while ((p = nexti++) != 0) {
		    // First update the steering vector
		    // adjustment is the conjugate of e times the input
		    // this is consistent with the RLSArray Star
			steering[i] = steering[i] + conj(e) * Complex((*p)%index);

		    // with the updated steering we can calculate the
		    // output simultaneously
			sum += (Complex)((*p)%0) * conj(steering[i++]);
		}
		output%0 << sum;
	}
	wrapup {
		const char* sf = SaveSteeringFile;
		if (sf != NULL && *sf != 0) {
			char* saveFileName = expandPathName(sf);
			// open the file for writing
			FILE* fp = fopen(saveFileName,"w");
			if (!fp) {
				Error::warn(*this, 
					"Cannot open file for writing: ",
					saveFileName,". Steering vector not saved.");
			} else {
				for (int i = 0; i < steering.size(); i++)
					fprintf(fp, "(%g ,%g)\n",
							steering[i].real(),
							steering[i].imag());
			}
			fclose(fp);
			delete [] saveFileName;
		}
	}
}

defstar {
	name {LMSArray}
	domain {SDF}
	desc {
An adaptive antenna array processor using the LMS algorithm.
The output signals of the antenna elements are taken
from a bus connected to the input.
	}
	version { $Id$ }
	author { U. Trautwein }
	copyright {
Copyright (c) 1996-1997 Technical University of Ilmenau.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF contribution library }
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
	setup {
	        // We have to account for errorDelay past input snapshots
		input.setSDFParams(1, int(errorDelay));
	}	
	go {
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

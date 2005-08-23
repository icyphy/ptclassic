defstar {
	name {RLSArrayStO}
	domain {SDF}
	desc {
An adaptive antenna array processor using the RLS algorithm.
The output signals of the antenna elements are taken
from a bus connected to the input.

The actual (time-dependent) steering vector is cyclically output, i.e.
on each invocation of the star you get one steering weight and after
length(steering) invocations you have one complete set of weights
which can be used e.g. for beam pattern calculation.

        }
	version { @(#)SDFRLSArrayStO.pl	1.4	05/28/98 }
	author { U. Trautwein and A. Richter }
	copyright {
Copyright (c) 1996- Technical University of Ilmenau.
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
		name { lambda }
		type { float }
		default { "0.97" }
		desc { exponential weighting factor }
	}
	defstate {
		name { delta }
		type { float }
		default { "0.1" }
		desc { initial correlation matrix value }
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
	ccinclude { <stdio.h> }
	protected {
		int cyclecounter;
		int NumberElements;
		// store one set of steering values for output
		Complex *outputset;
		Complex *p_matrix;
		Complex *t1_vector, *k_vector, *x_vector, *x_vector_new;
		Complex t2;
		double  lambda1;
	}
	constructor {
	    outputset = 0; 
	    p_matrix = t1_vector = k_vector = x_vector = x_vector_new = 0;
	}
	destructor {
	    LOG_DEL; delete [] outputset;
	    LOG_DEL; delete [] p_matrix;
	    LOG_DEL; delete [] t1_vector;
	    LOG_DEL; delete [] k_vector;
	    LOG_DEL; delete [] x_vector;
	    LOG_DEL; delete [] x_vector_new;
	}
	setup {
	        // We have to account for errorDelay past input snapshots
		input.setSDFParams(1, int(errorDelay));
		// delete previously allocated array
		LOG_DEL; delete [] outputset;
		// allocate memory
		NumberElements = steering.size();
		LOG_NEW; outputset = new Complex[NumberElements];
		int i;
		for (i = 0; i < NumberElements; i++)
		       outputset[i] = steering[i];
		cyclecounter = 0;

		// create some internal variables
		LOG_DEL; delete [] p_matrix;
		LOG_NEW; p_matrix = new Complex[NumberElements*NumberElements];
		LOG_DEL; delete [] t1_vector;
		LOG_NEW; t1_vector = new Complex[NumberElements];
		LOG_DEL; delete [] k_vector;
		LOG_NEW; k_vector = new Complex[NumberElements];
		LOG_DEL; delete [] x_vector;
		LOG_NEW; x_vector = new Complex[NumberElements];
		LOG_DEL; delete [] x_vector_new;
		LOG_NEW; x_vector_new = new Complex[NumberElements];

		lambda1=1.0/double(lambda);
		// P=1/delta*eye(N)
		int j, i1;
		for (i = i1 = 0; i < NumberElements; i++)
		   for (j = 0; j < NumberElements; j++)
		      p_matrix[i1++] = (i==j) ? Complex(1.0/double(delta),0.0) :
		         Complex(0.0,0.0);
	        
	}

	go {
	        StO%0 << outputset[cyclecounter]; 
	        // Are we one cycle through with steering vector output?
		cyclecounter++;
		if (cyclecounter == NumberElements) {
		    cyclecounter = 0;
		    for (int i = 0; i < NumberElements; i++)
		       outputset[i] = steering[i];
		   };

		int index = int(errorDelay);
		MPHIter nexti(input);
		PortHole *p;
		int i = 0;
		while ((p = nexti++) != 0)  {
			// We use a temporary variable to 
		        // avoid gcc2.7.2/2.8 problems
			Complex tmp1 = (*p)%index;
			x_vector[i] = tmp1;
			Complex tmp2 = (*p)%0;
			x_vector_new[i++] = tmp2;
		}
		int i1, j;

		// k=P*x/(lambda+x'*P*x)
		for (i=i1=0; i<NumberElements; i++) {
		    t1_vector[i] = Complex(0.0,0.0);
		    for (j=0; j<NumberElements; j++)
		      t1_vector[i] += p_matrix[i1++] * x_vector[j];
		};
		t2 = Complex(double(lambda),0.0);
		for (i=0; i<NumberElements; i++)
		   t2+=conj(x_vector[i])*t1_vector[i];
		t2=Complex(1.0,0.0)/t2;

		for (i=0; i<NumberElements; i++)
		   k_vector[i]=t1_vector[i]*t2;

		// w=w+k*conj(e)
		for (i=0; i<NumberElements; i++) {
		   // We use a temporary variable to 
	 	   // avoid gcc2.7.2/2.8 problems
		   Complex tmp = error%0;
		   steering[i]+=k_vector[i]*conj(tmp);
		}

		// y=w'*x (calculate the output signal, use the actual input)
		Complex y = Complex(0.0,0.0);
		for (i=0; i<NumberElements; i++)
		    y+=conj(steering[i])*x_vector_new[i];
		output%0 << y;

		// P=1/lambda*(P-k*x'*P)
		for (i=0; i<NumberElements; i++)
		   t1_vector[i]=Complex(0.0,0.0);
		for (i=i1=0; i<NumberElements; i++)
		   for (j=0; j<NumberElements; j++)
		      t1_vector[j]+=conj(x_vector[i])*p_matrix[i1++];

		for (i=i1=0; i<NumberElements; i++)
		   for (j=0; j<NumberElements; j++) {
		      p_matrix[i1]-=k_vector[i]*t1_vector[j];
		      p_matrix[i1++]*=Complex(lambda1,0.0);
		   }		
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

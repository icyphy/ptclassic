ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  D. G. Messerschmitt and E. A. Lee
 Date of creation: 1/16/90
 Revisions: 9/19/90, by EAL, to conform with Gabriel's adaptive filter.
 Converted to use preprocessor, 10/2/90 by J. Buck

 Implements an adaptive filter using the LMS tap update algorithm.'

**************************************************************************/
}

defstar {
	name { LMS }
	domain { SDF }
	derivedFrom { FIR }
	desc {
	    "Adaptive filter using LMS adaptation algorithm.\n"
	    "Initial coefficients are in the 'taps' state variable.\n"
	    "Default initial coefficients give an 8th order, linear phase\n"
	    "lowpass filter.  To read default coefficients from a file,\n"
	    "replace the default coefficients with \"<fileName\".\n"
	    "Supports the same sample-rate conversions as FIR."
	}
	input {
		name { error }
		type { float }
	}
	defstate {
		name { stepSize }
		type { float }
		default { "0.01" }
		desc { "step size" }
	}
	defstate {
		name { errorDelay }
		type { int }
		default { "1" }
		desc { "delay in the update loop" }
	}
	defstate {
		name { saveTapsFile }
		type { string }
		default { "" }
		desc { "file to save final tap values" }
	}
	ccinclude { "miscFuncs.h" }
	constructor {
		// remove interpolation as a settable parameter
		interpolation.clearAttributes(A_SETTABLE);
		// taps are no longer constant
		taps.clearAttributes(A_CONSTANT);
	}
	start {
	// First check to be sure that interpolation is 1.
		if (int(interpolation) != 1) {
			StringList msg = readFullName();
			msg += "Sorry, interpolation not supported yet";
			Error::abortRun (msg);
			return;
		}
		// Next run the FIR start routine
		SDFFIR :: start();

		// Then reset the signalIn number of samples in the past
		// to account for the error delay.
		signalIn.setSDFParams(int(decimation),
				      int(decimation) + 1 + int(errorDelay)
				      + taps.size());
	}
	go {
		// First update the taps
		double e = double(error%0);
		int index = int(errorDelay)*int(decimation) + int(decimationPhase);

		for (int i = 0; i < taps.size(); i++) {
			taps[i] = double(taps[i]) +
				(e * float(signalIn%(index))) * double(stepSize);
			index++;
		}
		
		// Then run FIR filter
		SDFFIR :: go();
	}
	wrapup {
		const char* sf = saveTapsFile;
		if (sf != NULL && *sf != 0) {
			const char* saveFileName = savestring (expandPathName(sf));
			// open the file for writing
			FILE* fp;
			if (!(fp = fopen(saveFileName,"w"))) {
				// File cannot be opened
				StringList msg = readFullName();
				msg += ": WARNING: Cannot open saveTapsFile for writing: ";
				msg += saveFileName;
				msg += ". Taps not saved.";
				errorHandler.error (msg);
			} else
				for (int i = 0; i < taps.size(); i++)
					fprintf(fp, "%d %g\n", i, taps[i]);
			fclose(fp);
			delete saveFileName;
		}
	}
}

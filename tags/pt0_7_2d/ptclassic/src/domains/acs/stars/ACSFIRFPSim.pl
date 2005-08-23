defcore {
	name { FIR }
	domain { ACS }
	coreCategory { FPSim }
	corona { FIR }
	desc {
A finite impulse response (FIR) filter.  Coefficients are specified by
the "taps" parameter.  The default coefficients give an 8th-order, linear
phase lowpass filter. To read coefficients from a file, replace the default
coefficients with "&lt; fileName", preferably specifying a complete path.
Rational sampling rate changes, implemented by polyphase multirate filters,
is also supported.
	}
	version { @(#)ACSFIRFPSim.pl	1.2 09/08/99 }
	author { James Lundblad }
    	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
	protected {
		int phaseLength;
	}
	setup {
		int d = corona.decimation;
		int i = corona.interpolation;
		int dP = corona.decimationPhase;
		corona.input.setSDFParams(d, d+1+(corona.taps.size()/i));
		corona.output.setSDFParams(i, i-1);
		if (dP >= d) {
			Error::abortRun (*this, ": decimationPhase too large");
			return;
		}
		// The phaseLength is ceiling(corona.taps.size/interpolation)
		// It is a protected instance variable.
		phaseLength = corona.taps.size() / i;
		if ((corona.taps.size() % i) != 0) phaseLength++;
	}
	go {
	    int Interp = corona.interpolation;
	    int Decim = corona.decimation;
	    int outCount = Interp - 1;
	
	    // phase keeps track of which phase of the filter coefficients
	    // are used. Starting phase depends on the decimationPhase state.
	    int phase = Decim - int(corona.decimationPhase) - 1;   
	
	    // Interpterate once for each input consumed
	    for (int inC = 1; inC <= Decim; inC++) {
		// Produce however many outputs are required
		// for each input consumed
		while (phase < Interp) {
		   double out = 0.0;
		   // Compute the inner product.
		   for (int i = 0; i < phaseLength; i++) {
			int tapsIndex = i * Interp + phase;
			double tap = 0.0;
			if (tapsIndex < corona.taps.size()) tap = corona.taps[tapsIndex];
			out += tap * double(corona.input%(Decim - inC + i));
		   }
		   // note: output%0 is the last output chronologically
		   corona.output%(outCount--) << out;
		   phase += Decim;
		}
		phase -= Interp;
	    }
	}
}

defstar {
	name {Lattice}
	domain {SDF}
	desc {
A Forward Lattice filter.
The "K" coefficients are in the "taps" state variable.
Default coefficients give a random filter.
To read coefficients from a file, replace the default
coefficients with "<fileName".
	}
	version {$Id$}
	author { Alan Kamas}
	copyright { 1992 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
.pp
This star implements a lattice filter.
To load filter coefficients from a file,
simply replace the default coefficients with the string "<filename".
.pp
It is advisable not to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the Lattice filter to work as expected regardless of
the directory in which the ptolemy process actually runs.
It is best to use tilde's in the filename to reference them to user's
home directory.  This way, future filesystem reorganizations
will have minimal effect.
	}
	seealso { FIR, ComplexFIR, BiQuad}
	input {
		name {signalIn}
		type {float}
	}
	output {
		name {signalOut}
		type {float}
	}
	defstate {
		name {taps}
		type {floatarray}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values. }
	}
	protected {
		double *w;
		double *y;
		int M;	
	}
	constructor {
		w = 0; y = 0; M = -1;
	}
	destructor {
		LOG_DEL; delete w;
		LOG_DEL; delete y;
	}
	start {
		// reallocate arrays only if size has changed,
		// or this is the first run.
		if (M != taps.size()) {
			M = taps.size();
			LOG_DEL; delete w;
			LOG_NEW; w = new double[M];
			LOG_DEL; delete y;
			LOG_NEW; y = new double[M+1];
		}
		for (int i=0; i < M; i++)  w[i]=0.0 ;
	}
	go {
		double k;
		
		// Forward:  Compute each of the Y values
		y[0] = double (signalIn%0);   // y(0)=x(n)
		for (int i=1; i <= M; i++) {
			k = taps[i-1];
			y[i] = k * w[i-1] + y[i-1];
		}
		signalOut%0 << y[M];

		// Backward:  Compute the w's for the next round
		for (i = M-1; i >0 ; i--) {
			k = taps[i-1];
			w[i] = k * y[i-1] + w[i-1];	
		}
		w[0] = double (signalIn%0);   // w[0]=x[n] 
			 
	}	
}

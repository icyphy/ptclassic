defstar {
	name {RLattice}
	domain {SDF}
	desc {
A Recursive (Backward) (IIR) Lattice filter.
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
This star implements Recursive lattice filter.
To load filter coefficients from a file,
simply replace the default coefficients with the string "<filename".
.br
.nf
.na
The Coefficients are as follows:

     y[0]          y[1]                 y[n-1]           y[n]
X(n) ---(+)->--o-->----(+)->--o--->-- ... ->--(+)->--o--->---o--->  Y(n)
          \   /          \   /                  \   /        |
         +Kn /        +Kn-1 /                  +K1 /         |
            X              X                      X          |
         -Kn \        -Kn-1 \                  -K1 \         V
          /   \          /   \                  /   \        |
        (+)-<--o--[z]--(+)-<--o--[z]- ... -<--(+)-<--o--[z]--/    
               w[1]           w[2]                   w[n]

where the [z] are unit delays and the (+) are adders.
      and "y" and "z" are defined in the code.
.fi
.ad
.pp
The Coefficients as entered in the "edit parameters" or in a file are,
left to right, K1 to Kn as above.
Note that the coefficent order and naming follows the convention of
both Oppenheim & Schafer and Jackson.
.pp
It is advisable not to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the Lattice filter to work as expected regardless of
the directory in which the ptolemy process actually runs.
It is best to use tilde's in the filename to reference them to user's
home directory.  This way, future filesystem reorganizations
will have minimal effect.
	}
	seealso { IIR, Lattice }
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
			LOG_NEW; w = new double[M+1];
			LOG_DEL; delete y;
			LOG_NEW; y = new double[M+1];
		}
		for (int i=0; i <= M; i++)  w[i]=0.0 ;
	}
	go {
		double k;
		
		// Forward:  Compute each of the Y values
		y[0] = double (signalIn%0);   // y(0)=x(n)
		for (int i=1; i <= M; i++) {
			k = taps[M-i];
			y[i] = k * w[i] + y[i-1];
		}
		signalOut%0 << y[M];

		// Backward:  Compute the w's for the next round
		for (i = 1; i < M ; i++) {
			k = -1 * taps[M-1-i];
			w[i] = w[i+1] + k*y[i+1];	
		}
		w[M] = y[M];
			 
	}	
}

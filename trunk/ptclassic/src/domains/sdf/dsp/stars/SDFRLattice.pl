defstar {
	name {RLattice}
	domain {SDF}
	desc {
A Recursive (Backward) (IIR) Lattice filter.  The default coefficient
implement the synthesis filter for a particular 4th order AR random process.
To read reflection coefficients from a file, replace the default
coefficients with "<fileName", preferably specifying a complete path.
	}
	version {$Id$}
	author { Alan Kamas}
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
.pp
This star implements a recursive lattice filter.
To load filter coefficients from a file,
simply replace the default coefficients with the string "<filename".
It is advisable not to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the Lattice filter to work as expected regardless of
the directory in which the ptolemy process actually runs.
It is best to use tilde's in the filename to reference them to user's
home directory.  This way, future filesystem reorganizations
will have minimal effect.
.pp
.nf
.na
The structure is as follows:
.cs R 18

     y[0]          y[1]                 y[n-1]           y[n]
X(n) ---(+)->--o-->----(+)->--o--->-- ... ->--(+)->--o--->---o--->  Y(n)
          \\   /          \\   /                  \\   /        |
         +Kn /        +Kn-1 /                  +K1 /         |
            X              X                      X          |
         -Kn \\        -Kn-1 \\                  -K1 \\         V
          /   \\          /   \\                  /   \\        |
        (+)-<--o--[z]--(+)-<--o--[z]- ... -<--(+)-<--o--[z]--/    
               w[1]           w[2]                   w[n]

.cs R
.fi
where the [z] are unit delays and the (+) are adders.
and "y" and "z" are defined in the code.
.ad
.pp
The reflection (or PARCOR) coefficients should be specified
left to right, K1 to Kn as above.
Using exactly the same coefficients in the
.c Lattice
star will result in precisely the inverse transfer function.
The default reflection coefficients give the following transfer function:
.EQ
H(z) ~=~ 1 over { 1 ~-~ 2 z sup -1 ~+~ 1.91z sup -2 ~-~ 0.91z sup -3 ~+~
0.205z sup -4 } ~.
.EN
Hence, the same coefficients in the
.c Lattice
star will give transfer function $H sup -1 (z)$.
.pp
Note that the definition of reflection coefficients is not quite universal
in the literature.  The reflection coefficients in references [2] and [3]
are the negative of the ones used by this star, which
correspond to the definition in most other texts,
and to the definition of partial-correlation (PARCOR)
coefficients in the statistics literature.
The sign of the coefficients used in this star is appropriate for values
given by the LevDur or Burg stars.
.UH REFERENCES
.ip [1]
J. Makhoul, "Linear Prediction: A Tutorial Review",
\fIProc. IEEE\fR, Vol. 63, pp. 561-580, Apr. 1975.
.ip [2]
S. M. Kay, \fIModern Spectral Estimation: Theory & Application\fR,
Prentice-Hall, Englewood Cliffs, NJ, 1988.
.ip [3]
S. Haykin, \fIModern Filters\fR, MacMillan Publishing Company,
New York, 1989.
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
		name {reflectionCoefs}
		type {floatarray}
		default { "0.804534 -0.820577 0.521934 -0.205" }
		desc { Reflection or PARCOR coefficients. }
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
	setup {
		// reallocate arrays only if size has changed,
		// or this is the first run.
		if (M != reflectionCoefs.size()) {
			M = reflectionCoefs.size();
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
			k = reflectionCoefs[M-i];
			y[i] = k * w[i] + y[i-1];
		}
		signalOut%0 << y[M];

		// Backward:  Compute the w's for the next round
		for (i = 1; i < M ; i++) {
			k = - reflectionCoefs[M-1-i];
			w[i] = w[i+1] + k*y[i+1];	
		}
		w[M] = y[M];
	}	
}

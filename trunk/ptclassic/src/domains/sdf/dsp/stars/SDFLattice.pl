defstar {
	name {Lattice}
	domain {SDF}
	desc {
An FIR Lattice filter.
The default reflection coefficients form the optimal predictor for
a particular 4th-order AR random process.
To read other reflection coefficients from a file, replace the default
coefficients with "&lt; fileName", preferably specifying a complete path.
	}
	version {$Id$}
	author { Alan Kamas}
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	htmldoc {
<p>
This star implements a lattice filter.
To load reflection coefficients from a file,
simply replace the default coefficients with the string "&lt;filename".
It is advisable to specify a path relative to a known directory,
such as the user's home directory referenced by the tilde character
or a directory contained in a reliable environment variable.
This will allow the Lattice filter to work as expected regardless of
the directory in which Ptolemy actually runs and regardless of
future file system reorganizations.
<p>
The default reflection coefficients correspond to the optimal linear
predictor for an AR process generated by filtering white noise with
the following filter:
<pre>
                           1
H(z) =  --------------------------------------
        1 - 2z<sup>-1</sup> + 1.91z<sup>-2</sup> - 0.91z<sup>-3</sup> + 0.205z<sup>-4</sup>
</pre>
<p>
Since this filter is minimum phase, the transfer function of the lattice
filter is <i>H <sup>-</i>1<i></sup> </i>(<i>z</i>)<i></i>.
<p>
Note that the definition of reflection coefficients is not quite universal
in the literature.
The reflection coefficients in references [2] and [3]
are the negative of the ones used by this star, which
correspond to the definition in most other texts,
and to the definition of partial-correlation (PARCOR)
coefficients in the statistics literature.
The signs of the coefficients used in this star are appropriate for values
given by the LevDur and Burg stars.
<a name="Makhoul, J."></a>
<a name="Kay, S. M."></a>
<a name="Haykin, S."></a>
<h3>References</h3>
<p>[1]  
J. Makhoul, "Linear Prediction: A Tutorial Review",
<i>Proc. IEEE</i></b>, Vol. 63, pp. 561-580, Apr. 1975.
<p>[2]  
S. M. Kay, <i>Modern Spectral Estimation: Theory & Application</i></b>,
Prentice-Hall, Englewood Cliffs, NJ, 1988.
<p>[3]  
S. Haykin, <i>Modern Filters</i></b>, MacMillan Publishing Company,
New York, 1989.
	}
	seealso { RLattice, BlockLattice, BlockRLattice, FIR, FIRCx, Biquad}
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
		double *b;
		double *f;
		int M;	
	}
	constructor {
		b = 0; f = 0; M = -1;
	}
	destructor {
		LOG_DEL; delete [] b;
		LOG_DEL; delete [] f;
	}
	setup {
		// reallocate arrays only if size has changed,
		// or this is the first run.
		if (M != reflectionCoefs.size()) {
			M = reflectionCoefs.size();
			LOG_DEL; delete [] b;
			LOG_NEW; b = new double[M];
			LOG_DEL; delete [] f;
			LOG_NEW; f = new double[M+1];
		}
		for (int i=0; i < M; i++)  b[i]=0.0 ;
	}
	go {
		double k;
		
		// Forward prediction error
		f[0] = double (signalIn%0);   // f(0)=x(n)
		for (int i=1; i <= M; i++) {
			k = - reflectionCoefs[i-1];
			f[i] = k * b[i-1] + f[i-1];
		}
		signalOut%0 << f[M];

		// Backward:  Compute the w's for the next round
		for (i = M-1; i >0 ; i--) {
			k = - reflectionCoefs[i-1];
			b[i] = k * f[i-1] + b[i-1];	
		}
		b[0] = double (signalIn%0);   // b[0]=x[n] 
			 
	}	
}

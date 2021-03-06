defstar {
	name {LevDur}
	domain {SDF}
	desc {
This star uses the Levinson-Durbin algorithm to compute the linear predictor
coefficients of a random process, given its autocorrelation function as an
input.
These coefficients are produced both in autoregressive form
(on the "lp" output) and in lattice filter form (on the "refl" output).
The "errPower" output is the power of the prediction error as a function
of the predictor order.
	}
	version {@(#)SDFLevDur.pl	1.22 10/06/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	htmldoc {
<p>
This star takes as inputs an autocorrelation function, or
estimates produced by the
<tt>Autocor</tt>
star, and uses the Levinson-Durbin
algorithm to compute both reflection coefficients and FIR linear
predictor coefficients.
<a name="spectral estimation, maximum entropy"></a>
<a name="maximum entropy spectral estimation"></a>
If the
<tt>Autocor</tt>
star is set so that it's <i>unbiased</i> parameter is zero,
then the combined effect of that star and this one is called the
autocorrelation algorithm.
<a name="autocorrelation method"></a>
The given order should be the same as the <i>noLags</i> parameter of the
<tt>Autocor</tt>
star.  Three outputs are generated.
On the <i>errPower</i> output, a sequence of <i>order  +  </i>1
samples gives the prediction
error power for each predictor order from zero to <i>order</i>.
The first sample, which corresponds to the zeroth-order predictor,
is simply an estimate of the power of the input process.
Note that for signals without noise, the <i>errPower</i> output can sometimes
end up being a small negative number.
<p>
The <i>lp</i> output gives the coefficients of an FIR filter that
performs linear prediction for the input process.
<a name="linear prediction"></a>
This set of coefficients is suitable for directly feeding a
<tt>BlockFIR</tt>
filter star that accepts outside coefficients.
The number of coefficients produced is equal to the <i>order</i>.
The final output <i>refl</i> is the reflection coefficients,
suitable for feeding directly to a
<tt>BlockLattice</tt>
star, which will then generate the forward and backward prediction error.
The number of coefficients produced is equal to the <i>order</i>.
<p>
Note that the definition of reflection coefficients is not quite
universal in the literature.
<a name="reflection coefficients"></a>
<a name="coefficients, reflection"></a>
The reflection coefficients in references [2] and [3]
are the negative of the ones generated by this star, which
correspond to the definition in most other texts,
and to the definition of partial-correlation (PARCOR)
coefficients in the statistics literature.
<a name="partial correlation coefficients"></a>
<a name="coefficients, partial correlation"></a>
<a name="PARCOR"></a>
<a name="Makhoul, J."></a>
<a name="Kay, S. M."></a>
<a name="Haykin, S."></a>
<h3>References</h3>
<p>[1]  
J. Makhoul, "Linear Prediction: A Tutorial Review",
<i>Proc. IEEE</i>, vol. 63, pp. 561-580, Apr. 1975.
<p>[2]  
S. M. Kay, <i>Modern Spectral Estimation: Theory & Application</i>,
Prentice-Hall, Englewood Cliffs, NJ, 1988.
<p>[3]  
S. Haykin, <i>Modern Filters</i>, MacMillan Publishing Company,
New York, 1989.
	}
	seealso { Autocor, BlockFIR, linearPrediction }
	input {
		name {autocor}
		type {float}
		desc { Autocorrelation estimate }
	}
	output {
		name {lp}
		type {float}
		desc { FIR linear predictor coefficients output. }
	}
	output {
		name {refl}
		type {float}
		desc { Lattice predictor coefficients output. }
	}
	output {
		name {errPower}
		type {float}
		desc { Prediction error power. }
	}
	defstate {
		name {order}
		type {int}
		default {8}
		desc {The order of the recursion.}
	}
	protected {
		double *aOrig, *aPrime, *r;
		int ORD;
	}
	constructor {
		aOrig = aPrime = r = 0;
		ORD = 0;
	}
	destructor {
		LOG_DEL; delete [] aOrig;
		LOG_DEL; delete [] aPrime;
		LOG_DEL; delete [] r;
	}
	setup {
		// delete and remake arrays if the size has changed.
		if (int(order) != ORD) {
			LOG_DEL; delete [] aOrig;
			LOG_DEL; delete [] aPrime;
			LOG_DEL; delete [] r;
			ORD = int(order);
			LOG_NEW; aOrig = new double[ORD+1];
			LOG_NEW; aPrime = new double[ORD+1];
			LOG_NEW; r = new double[ORD+1];
		}
		refl.setSDFParams (ORD, ORD-1);
		lp.setSDFParams (ORD, ORD-1);
		errPower.setSDFParams (ORD+1, ORD);
		autocor.setSDFParams (2*ORD, ORD);
	}
	go {
	    // Define pointers so that the arrays can be swapped
	    double* a = aOrig;
	    double* aP = aPrime;
	    double gamma;
	    int m;

	    // for convenience, read the autocorrelation lags into a vector
	    for (int i = 0; i <= ORD; i++) {
		r[i] = double(autocor%(ORD-i));
	    }
	    // Initial prediction error is simply the zero-lag of
	    // of the autocorrelation, or the signal power estimate.
	    double P = r[0];

	    // Output the zeroth order prediction error power, which is
	    // simply the power of the input process
	    errPower%ORD << r[0];

	    // First coefficient is always unity
	    a[0] = 1.0;
	    aP[0] = 1.0;

	    // The order recurrence
	    for (int M = 0; M < ORD; M++ ) {

		// Compute the new reflection coefficient
		double deltaM = 0.0;
		for (m = 0; m < M+1; m++) {
		    deltaM += a[m]*r[M+1-m];
		}
		// Compute and output the reflection coefficient
		// (which is also equal to the last AR parameter)
		aP[M+1] = gamma = -deltaM/P;
		refl%(ORD-M-1) << - double(gamma);

		for (m = 1; m < M+1; m++) {
		    aP[m] = a[m] + gamma*a[M+1-m];
		}

		// Update the prediction error power
		P = P*(1.0 - gamma*gamma);
		errPower%(ORD-M-1) << double(P);

		// Swap a and aP for next order recurrence
		double* temp = a;
		a = aP;
		aP = temp;
	    }
	    // generate the lp outputs
	    for (m = 1; m <= ORD; m++ ) {
		lp%(ORD-m) << -a[m];
	    }
	}
}

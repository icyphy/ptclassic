ident {
/**************************************************************************
Version identification:
$Id$

 Programmer:  E. A. Lee
 Date of creation: 10/28/90

This star takes inputs from the
.c Autocor
star and uses the Levinson-Durbin
algorithm to compute both reflection coefficients and FIR linear
predictor coefficients.
If the
.c Autocor
star is set so that it's \fIunbiased\fR parameter is zero,
then the combined effect of that star and this one is called the
autocorrelation algorithm.
.IE "autocorrelation algorithm"
The given order should be the same as the \fInoLags\fR parameter of the
.c Autocor
star.  Three outputs are generated.
On the \fIerrPower\fR output, a sequence of $order ~+~ 1$
samples gives the prediction
error power for each predictor order from zero to \fIorder\fR.
The first sample, the corresponding to the zeroth order predictor,
is simply an estimate of the power of the input process.
Note that for signals without noise, the \fIerrPower\fR output can sometimes
end up being a small negative number.
.pp
The \fIlp\fR output gives the coefficients of an FIR filter that
performs linear prediction for the input process.
This set of coefficients is suitable for directly feeding a
.c BlockFIR
filter star that accepts outside coefficients.
The number of coefficients produced is equal to the \fIorder\fR.
The final output \fIrefl\fR is the reflection coefficients,
suitable for feeding directly to a
.c BlockLattice
star, which will then generate the forward and backward prediction error.
The number of coefficients produced is equal to the \fIorder\fR.
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


SEE ALSO
Autocor,
BlockFIR,
linearPrediction.


**************************************************************************/

#define MAXORDER 256
}
defstar {
	name {LevDur}
	domain {SDF}
	desc { "Levinson-Durbin algorithm" }
	input {
		name {autocor}
		type {float}
		// desc { "Autocorrelation estimate" }
	}
	output {
		name {lp}
		type {float}
		// desc { "FIR linear predictor coefficients output" }
	}
	output {
		name {refl}
		type {float}
		// desc { "Lattice predictor coefficients output" }
	}
	output {
		name {errPower}
		type {float}
		// desc { "Prediction error power" }
	}
	defstate {
		name {order}
		type {int}
		default {8}
		desc {"The order of the recursion"}
	}
	start {
		if (int(order) > MAXORDER) {
			Error::abortRun(*this,
				": Maximum order in LevDur exceeded");
			return;
		}
		refl.setSDFParams (int(order), int(order)-1);
		lp.setSDFParams (int(order), int(order)-1);
		errPower.setSDFParams (int(order)+1, int(order));
		autocor.setSDFParams (2*int(order), int(order));
	}
	go {
	    double aOrig[MAXORDER];
	    double aPrime[MAXORDER];
	    // Define pointers so that the arrays can be swapped
	    double* a = aOrig;
	    double* aP = aPrime;
	    double gamma;
	    double r[MAXORDER];
	    int m;

	    // for convenience, read the autocorrelation lags into a vector
	    for (int i = 0; i <= int(order); i++) {
		r[i] = float(autocor%(int(order)-i));
	    }
	    // Initial prediction error is simply the zero-lag of
	    // of the autocorrelation, or the signal power estimate.
	    double P = r[0];

	    // Output the zeroth order prediction error power, which is
	    // simply the power of the input process
	    errPower%(int(order)) << r[0];

	    // First coefficient is always unity
	    a[0] = 1.0;
	    aP[0] = 1.0;

	    // The order recurrence
	    for (int M = 0; M < int(order); M++ ) {

		// Compute the new reflection coefficient
		double deltaM = 0.0;
		for (m = 0; m < M+1; m++) {
		    deltaM += a[m]*r[M+1-m];
		}
		// Compute and output the reflection coefficient
		// (which is also equal to the last AR parameter)
		aP[M+1] = gamma = -deltaM/P;
		refl%(int(order)-M-1) << - double(gamma);

		for (m = 1; m < M+1; m++) {
		    aP[m] = a[m] + gamma*a[M+1-m];
		}

		// Update the prediction error power
		P = P*(1.0 - gamma*gamma);
		errPower%(int(order)-M-1) << double(P);

		// Swap a and aP for next order recurrence
		double* temp = a;
		a = aP;
		aP = temp;
	    }
	    // generate the lp outputs
	    for (m = 1; m <= int(order); m++ ) {
		lp%(int(order)-m) << -a[m];
	    }
	}
}

ident {
/**************************************************************************
Version identification:
$Id$

 Programmer:  E. A. Lee
 Date of creation: 10/31/90

This star uses Burg's algorithm to estimate the reflection coefficients
and AR parameters of an input random process.
The number of inputs looked at is given by the \fInumInputs\fR parameter
and the order of the AR model is given by the \fIorder\fR parameter.
The order specifies how many outputs appear on the \fIlp\fR and
\fIrefl\fR output portholes.
These outputs are, respectively, the autoregressive (AR) parameters
(also called the linear predictor parameters),
and the reflection coefficients.
.pp
Note that the definition of reflection coefficients is not quite
standard.  The reflection coefficients in references [2] and [3]
are the negative of the ones generated by this star.
The ones generated correspond to the definition in most other texts,
and correspond to the definition of partial-correlation (PARCOR)
coefficients in the statistics literature.
.pp
The \fIerrPower\fR output is the power of the prediction error
as a function of the model order.  There are $order+1$ output samples,
where the first corresponds to the prediction error of a zero-th
order predictor.  This is simply an estimate of the input signal power.
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
.SA
LevDur,
linearPrediction,
powerSpectrum.

**************************************************************************/

#define MAXORDER 256
#define MAXNOINPUTS 1024
}
defstar {
	name {Burg}
	domain {SDF}
	desc { "Burg's algorithm" }
	input {
		name {input}
		type {float}
		// desc { "Input random process" }
	}
	output {
		name {lp}
		type {float}
		// desc { "AR coefficients output" }
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
		desc {"The number of reflection coefficients to generate"}
	}
	defstate {
		name {numInputs}
		type {int}
		default {64}
		desc {"The number of inputs used to estimate the model"}
	}
	start {
		if (int(order) > MAXORDER) {
			Error::abortRun(*this,
				": Maximum order in Burg algorithm exceeded");
			return;
		}
		if (int(numInputs) > MAXNOINPUTS) {
			Error::abortRun(*this,
				": Maximum number of inputs in Burg exceeded");
			return;
		}
		refl.setSDFParams (int(order), int(order)-1);
		lp.setSDFParams (int(order), int(order)-1);
		errPower.setSDFParams (int(order)+1, int(order));
		input.setSDFParams (int(numInputs), int(numInputs)-1);
	}
	go {
	    double f[MAXNOINPUTS];	// forward predictor error estimates
	    double b[MAXNOINPUTS];	// backward predictor error estimates
	    double aOrig[MAXORDER];	// FIR predictor coefficients
	    double aPrime[MAXORDER];	// FIR predictor coefficients
	    // Define pointers so that the arrays can be swapped
	    double* a = aOrig;
	    double* aP = aPrime;
	    double gamma, dsum, nsum;

	    // initialize the forward and backward predictor errors
	    // and the prediction error power estimate
	    int count = 0;
	    double ep = 0.0;	// error power estimate
	    for (int i = int(numInputs)-1; i >= 0; i--) {
		f[count] = input%i;
		b[count++] = input%i;
		ep += float(input%i) * (float(input%i));
	    }
	    ep = ep/int(numInputs);
	    // output the zeroth order prediction error, which is simply
	    // the power estimate of the input
	    errPower%(int(order)) << ep;

	    // Iterate on the predictor order
	    for (int m = 1; m <= int(order); m++ ) {
	        // Compute the reflection coefficients, and output them
		nsum = 0.0;
		dsum = 0.0;
		for (i = m; i < int(numInputs); i++) {
		    nsum += f[i]*b[i-1];
		    dsum += f[i]*f[i] + b[i-1]*b[i-1];
		}
		gamma = -2*nsum/dsum;
		refl%(int(order)-m) << - gamma;

		// update the forward and backward predictor errors
		for (i = int(numInputs)-1; i >= m; i--) {
		    float tempf = f[i];
		    f[i] = tempf + gamma*b[i-1];
		    b[i] = b[i-1] + gamma*tempf;
		}

		// update the prediction error power estimate
		ep = (1 - gamma*gamma) * ep;
		errPower%(int(order)-m) << ep;

		// Update the FIR predictor coefficient estimates
		for (i = 1; i < m; i++) {
		    aP[i] = a[i] + gamma * a[m-i];
		}
		aP[m] = gamma;

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

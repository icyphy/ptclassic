defstar {
	name { Autocor }
	domain {SDF}
	version {$Id$}
	desc { Estimates an autocorrelation by averaging input samples. }
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
.pp
Estimates a certain number of samples of the autocorrelation of the input
.Id "autocorrelation"
by averaging a certain number of input samples.  The number of outputs
is twice the number of lags requested.  This makes the output almost
symmetric (discard the last sample to get a perfectly symmetric output).
.pp
If the parameter \fIunbiased\fR is NO, then
the autocorrelation estimate is
.Id "unbiased autocorrelation"
.EQ
r hat (k) ~=~ 1 over N sum from n=0 to N-1-k x(n)x(n+k)
.EN
for $k ~=~ 0, ... , ~ p$, where $N$ is the number of inputs to average
(\fInoInputsToAvg\fR) and $p$ is the number of lags to estimate (\fInoLags\fR).
This estimate is biased because the outermost lags have fewer than $N$
.Id "biased autocorrelation"
terms in the summation, and yet the summation is still normalized by $N$.
.pp
If the parameter \fIunbiased\fR is YES (the default), then the estimate is
.EQ
r hat (k) ~=~ 1 over N-k sum from n=0 to N-1-k x(n)x(n+k) ~.
.EN
In this case, the estimate is unbiased.
However, note that the unbiased estimate does not guarantee
a positive definite sequence, so a power spectral estimate based on this
autocorrelation estimate may have negative components.
.Ir "spectral estimation"
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name {noInputsToAvg}
		type {int}
		default {"256"}
		desc {Number of input samples to average.}
	}
	defstate {
		name {noLags}
		type {int}
		default {"64"}
		desc { Number of autocorrelation lags to output.}
	}
	defstate {
		name {unbiased}
		type {int}
		default {"YES"}
		desc { If YES, the estimate will be unbiased.}
	}
	start {
	    if ( int(noInputsToAvg) <= int(noLags)) {
		Error::abortRun(*this,
			": noLags is larger than noInputsToAvg.");
		return;
	    }
	    input.setSDFParams(int(noInputsToAvg), int(noInputsToAvg)-1);
	    output.setSDFParams(2*int(noLags), 2*int(noLags));
	}
	go {
	    for(int i = int(noLags); i>=0; i--) {
		double sum = 0.0;
		for(int j = 0; j < (int(noInputsToAvg)-i); j++)
		   sum += double(input%j) * double(input%(j+i));
		if (unbiased)
		    output%(i+int(noLags)) << sum/(int(noInputsToAvg) -i);
		else
		    output%(i+int(noLags)) << sum/int(noInputsToAvg);
	    }
	    // Now output the second half, which by symmetry is just
	    // identical to what was just output.
	    for(i = int(noLags)-1; i >= 0; i--)
		output%i = output%(2*int(noLags) - i);
	}
}

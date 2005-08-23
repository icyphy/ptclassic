defstar {
	name { Autocor }
	domain {SDF}
	version {@(#)SDFAutocor.pl	2.19 05/04/98}
	desc { 
Estimate an autocorrelation function by averaging input samples.
Both biased and unbiased estimates are supported.  By default,
the last sample in the autocorrelation sequence is not output
so as to make the sequence have an even number of samples.
	}
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
Estimates a certain number of samples of the autocorrelation of the input
<a name="autocorrelation"></a>
by averaging a certain number of input samples.
<p>
If the parameter <i>makeSymmetric</i> is NO, then the output
will be symmetric and contain a number of samples equal to twice
the number of lags requested plus one.  Otherwise, the output
will be twice the number of lags requested which will be almost
symmetric (discard the last sample to get a perfectly symmetric output).
<p>
If the parameter <i>unbiased</i> is NO, then
the autocorrelation estimate is
<a name="unbiased autocorrelation"></a>
<pre>
         N-1-k
       1  --- 
r(k) = -  \    x(n)x(n+k)
       N  /
          ---
          n=0
</pre>
for <i>k </i>=0<i>, ... , p</i>, where <i>N</i> is the number of
inputs to average (<i>noInputsToAvg</i>) and <i>p</i> is the number of
lags to estimate (<i>noLags</i>).
This estimate is biased because the outermost lags have fewer than <i>N</i>
<a name="biased autocorrelation"></a>
terms in the summation, and yet the summation is still normalized by <i>N</i>.
<p>
If the parameter <i>unbiased</i> is YES (the default), then the estimate is
<pre>
           N-1-k
        1   ---
r(k) = ---  \    x(n)x(n+k)
       N-k  /
            ---
            n=0
</pre>
In this case, the estimate is unbiased.
However, note that the unbiased estimate does not guarantee
a positive definite sequence, so a power spectral estimate based on this
autocorrelation estimate may have negative components.
<a name="spectral estimation"></a>
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
	defstate {
		name {makeSymmetric}
		type {int}
		default {"NO"}
		desc {
If YES, then the star will output 2*noLags+1 samples (an odd number)
whose values are symmetric about the midpoint.
If NO, then the star will output 2*noLags samples (an even number)
by omitting one of the endpoints. }
	}
	setup {
	    if ( int(noInputsToAvg) <= int(noLags)) {
		Error::abortRun(*this,
			": noLags is larger than noInputsToAvg.");
		return;
	    }

	    input.setSDFParams(int(noInputsToAvg), int(noInputsToAvg)-1);

	    // 2*int(noLags)+1 samples of the autocorrelation sequence
	    // are always computed so the buffer size on the output port
	    // is always 2*int(noLags)+1 samples.  This buffer size is
	    // specified by the second argument to setSDFParams.
	    // The number of samples produced is either 2*int(noLags) or
	    // 2*int(noLags)+1 depending on the value of makeSymmetric. BLE
	    if ( int(makeSymmetric) )
	        output.setSDFParams(2*int(noLags)+1, 2*int(noLags));
	    else
	        output.setSDFParams(2*int(noLags), 2*int(noLags));
	}
	go {
	    int i;
	    for(i = int(noLags); i>=0; i--) {
		double sum = 0.0;
		for(int j = 0; j < (int(noInputsToAvg)-i); j++)
		   sum += double(input%j) * double(input%(j+i));
		if ( int(unbiased) )
		    output%(i+int(noLags)) << sum/(int(noInputsToAvg) - i);
		else
		    output%(i+int(noLags)) << sum/int(noInputsToAvg);
	    }
	    // Now output the second half, which by symmetry is just
	    // identical to what was just output.
	    for(i = int(noLags)-1; i >= 0; i--)
		output%i = output%(2*int(noLags) - i);
	}
}

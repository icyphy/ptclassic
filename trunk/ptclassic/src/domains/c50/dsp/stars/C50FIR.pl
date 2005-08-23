defstar {
	name { FIR }
	domain { C50 }
	desc {
Finite Impulse Response (FIR) filter.
Coefficients are in the "taps" state variable.  Default coefficients
give an 8th order, linear phase lowpass filter.  To read coefficients
from a file, replace the default coefficients with "<fileName".
	}
	version {@(#)C50FIR.pl	1.14	05/26/98}
	author { Luis Gutierrez, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
<a name="fir"></a>
The output of the FIR filter is given by:
<pre>
sum from i = 0 to i = (N - 1) of  c[i]&#183x[N-i]
</pre>
where N is the order of the filter given by the number of coefficients in
<i>taps</i>.
c[i], i = 0..N-1 are the tap coefficients; and x[T-i] is the input i samples
before the current input.
<p>
The default filter is a linear-phase equiripple lowpass filter with its 3dB
cutoff frequency at about 1/3 of the Nyquist frequency.
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {taps}
		type {FIXARRAY}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values }
		attributes { A_SETTABLE | A_CONSTANT | A_UMEM}
	}
	state {
		name {decimation}
		type {int}
		default {1}
		desc {Decimation ratio, currently not supported}
	}
        state {
		name {decimationPhase}
		type {int}
		default {0}
		desc {Downsampler phase, currently not supported.}
	}
	state {
		name {interpolation}
		type {int}
		default {1}
		desc {Interpolation ratio, currently not supported}
	}
	state {
		name {oldSamples}
		type {fixarray}
		default {0}
		desc {internal}
                attributes {A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
	}

	protected {
		int tapsNum;
		StringList tapInit;
	}

	setup {
		tapsNum = taps.size();
		if (tapsNum < 1) {
			Error::abortRun(*this,
			"There must be a positive number of taps");
		}
		oldSamples.resize(int(tapsNum)+1);
        }

       

	go {
		addCode(std());
	}


//FIXME: star does not support decimation or interpolation.
	codeblock(std,""){
	setc	ovm			; set overflow mode
	LAR	AR2,#$addr(oldSamples)	; AR0 -> start of old sample array
	LAR	AR1,#$addr(output)	; AR1 -> output signal
	lar	ar0,#($addr(oldSamples)+@(tapsNum-1))
	ZAP				; zero accumulator and product reg.
	MAR	*,AR2			; ARP = AR0
	BLDD	#$addr(input),*,ar0	; move (input) to first addr in array
	RPT	#@(int(tapsNum)-1)		; these two instructions 
	MACD	$addr(taps),*-	; implement the filter
	lta	*,ar1
	sacb
	addb				; saturate on overflow
	clrc	ovm			; unset overflow mode
	SACH	*			; save output
	}
	
	execTime  {
		return 10 + int(tapsNum);
	}
}


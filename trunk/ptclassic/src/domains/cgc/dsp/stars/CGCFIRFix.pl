defstar {
        name {FIRFix}
        domain {CGC}
	derivedFrom { CGCFix }
        desc {
A finite impulse response (FIR) filter with fixed-point capabilities.
The fixed-point coefficients are specified by the "taps" parameter.
The default coefficients give an 8th-order, linear phase lowpass filter.
To read coefficients from a file, replace the default coefficients
with "<fileName", preferably specifying a complete path.
Polyphase multirate filtering is also supported.
        }
        version { $Id$ }
        author { Edward A. Lee, Alireza Khazeni, J.Weiss }
        copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { CGC dsp library }
        explanation {
.pp
This star implements a finite-impulse response filter with multirate capability.
.Id "filter, FIR"
.Id "FIR filter"
The default coefficients correspond to an eighth-order, equiripple,
linear-phase, lowpass filter which has a 3 dB cutoff frequency at
approximately $1/3$ of the Nyquist frequency.
The default precision on these coefficients is a total of 24 bits.
The number of decimal bits is chosen as the minimum number of bits
needed to represent the number in fixed-point.
One bit is reserved for the sign, and the rest are fractional bits.
During computation of filter outputs, the precision of the filter taps
is converted to the precision contained in the "TapsPrecision" parameter.
.pp
To load the filter coefficients from a file, simply replace the default
coefficients with the string "<filename".
It is advisable to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the FIR filter to work as expected regardless of
the directory in which the ptolemy process actually runs.
It is best to use tilde characters in the filename to reference them to
the user's home directory.
This way, future file system reorganizations will have minimal effect.
.pp
When the \fIdecimation\fP (\fIinterpolation\fP)
state is different from unity, the filter behaves exactly
as it were followed (preceded) by a DownSample (UpSample) star.
However, the implementation is much more efficient than
it would be using UpSample and DownSample stars because
a polyphase structure is used internally, thereby avoiding the
unnecessary memory locations and multiplication by zero.
Arbitrary sample rate conversions by rational factors can
be accomplished in this way.
.pp
To design a filter for a multirate system, simply assume that the
sample rate is the product of the interpolation parameter and
the input sample rate, or equivalently, the product of the decimation
parameter and the output sample rate.
.Ir "multirate filter design"
.Ir "filter design, multirate"
.Ir "filter, multirate"
In particular, considerable care must be taken to avoid aliasing.
Specifically, if the input sample rate is f,
then the filter stopband should begin before $f/2$.
If the interpolation ratio is $i$, then $f/2$ is a fraction $1/(2 i)$
of the sample rate at which you must design your filter.
.pp
The \fIdecimationPhase\fP parameter is somewhat subtle.
It is exactly equivalent the phase parameter of the DownSample star.
Its interpretation is as follows; when decimating,
samples are conceptually discarded (although a polyphase structure
does not actually compute the discarded samples).
If you are decimating by a factor of three, then you will select
one of every three outputs, with one of three possible phases.
When decimationPhase is zero (the default),
the latest (most recent) samples are the ones selected.
The decimationPhase must be strictly less than
the decimation ratio.
.pp
For more information about polyphase filters, see [1-2].
.Id "Harris, F. J."
.Id "Vaidyanathan, P. P."
.UH "REFERENCES"
.ip [1]
F. J. Harris,
``Multirate FIR Filters for Interpolating and Desampling'', in
\fIHandbook of Digital Signal Processing\fR, Academic Press, 1987.
.ip [2]
P. P. Vaidyanathan,
``Multirate Digital Filters, Filter Banks, Polyphase
Networks, and Applications: A Tutorial'',
\fIProc. of the IEEE\fR, vol. 78, no. 1, pp. 56-93, Jan. 1990.
        }
        seealso { FIRCx, Biquad, UpSample, DownSample, firDemo, interp, multirate }
        input {
                name {signalIn}
                type {fix}
        }
        output {
                name {signalOut}
                type {fix}
        }
        defstate {
                name {taps}
                type {fixarray}
                default {
"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
                }
                desc { Filter tap values. }
        }
        defstate {
                name {decimation}
                type {int}
                default {1}
                desc {Decimation ratio.}
        }
        defstate {
                name {decimationPhase}
                type {int}
                default {0}
                desc {Downsampler phase.}
        }
        defstate {
                name {interpolation}
                type {int}
                default {1}
                desc {Interpolation ratio.}
        }
        defstate {
                name { ArrivingPrecision }
                type {int}
                default {"YES"}
                desc {
Flag indicating whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the precision specified
by the parameter "InputPrecision".
		}
        }
        defstate {
                name { InputPrecision }
                type { precision }
                default { 4.14 }
                desc {
Precision of the input in bits.
The input particles are only cast to this precision if the parameter
"ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { TapPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the taps in bits. }
        }
        defstate {
                name { AccumulationPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the accumulation in bits. }
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 4.14 }
                desc { Precision of the output in bits. } 
        }

	defstate {
		name {phaseLength}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {tapSize}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {tap}
		type {fix}
		default{0.0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {Accum}
		type {fix}
		default{0.0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}

	setup {
                CGCFix::setup();

		int d = decimation;
		int i = interpolation;
		int dP = decimationPhase;
		signalIn.setSDFParams(d, d+1+(taps.size()/i));
		signalOut.setSDFParams(i, i-1);
		if (dP >= d) {
			Error::abortRun (*this, ": decimationPhase too large");
			return;
		}
		// The phaseLength is ceiling(taps.size/interpolation)
		// It is a protected instance variable.
		int temp = taps.size() / i;
		tapSize = taps.size();
		if ((taps.size() % i) != 0)  temp++;
		phaseLength = temp;

		// Set the precision on the fixed-point variables

		if (!int(ArrivingPrecision))
			signalIn.setPrecision(InputPrecision);
		signalOut.setPrecision(OutputPrecision);

		tap.setPrecision(TapPrecision);
		Accum.setPrecision(AccumulationPrecision);
	}

   codeblock(bodyDecl) {
	int phase, tapsIndex, inC, i;
	int outCount = $val(interpolation) - 1;
	int inPos;
	double out, tap;
   }

   codeblock(body) {
	/* phase keeps track of which phase of the filter coefficients is used.
	   Starting phase depends on the decimationPhase state. */
	phase = $val(decimation) - $val(decimationPhase) - 1;   
	
	/* Iterate once for each input consumed */
	for (inC = 1; inC <= $val(decimation) ; inC++) {

		/* Produce however many outputs are required for each 
		   input consumed */
		while (phase < $val(interpolation)) {

			FIX_SetToZero($ref(Accum));

			/* Compute the inner product. */
			for (i = 0; i < $val(phaseLength); i++) {
				tapsIndex = i * $val(interpolation) + phase;
				if (tapsIndex >= $val(tapSize))
			    		FIX_SetToZero($ref(tap));
				else
			 		FIX_Assign($ref(tap),$ref(taps,tapsIndex));

				inPos = $val(decimation) - inC + i;
				FIX_MulAdd($ref(Accum), $ref(tap),$ref(signalIn,inPos));
			}
			FIX_Assign($ref(signalOut,outCount),$ref(Accum));
			outCount--;;
			phase += $val(decimation);
		}
		phase -= $val(interpolation);
	}
   }

	go {
		addCode(bodyDecl);
		addCode(body);
	}
        // A wrap-up method is inherited from CGCFix
        // If you defined your own, you should call CGCFix::wrapup()
}

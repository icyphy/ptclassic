defstar {
        name {FIRFix}
        domain {SDF}
	derivedFrom { SDFFix }
        desc {
A finite impulse response (FIR) filter with fixed-point capabilities.
The fixed-point coefficients are specified by the "taps" parameter.
The default coefficients give an 8th-order, linear phase lowpass filter.
To read coefficients from a file, replace the default coefficients
with "&lt;fileName", preferably specifying a complete path.
Polyphase multirate filtering is also supported.
        }
        version { @(#)SDFFIRFix.pl	1.25	09/01/97 }
        author { Edward A. Lee, Alireza Khazeni }
        copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { SDF dsp library }
	htmldoc {
<p>
This star implements a finite-impulse response filter with multirate capability.
<a name="filter, FIR"></a>
<a name="FIR filter"></a>
The default coefficients correspond to an eighth-order, equiripple,
linear-phase, lowpass filter which has a 3 dB cutoff frequency at
approximately 1/3 of the Nyquist frequency.
The default precision on these coefficients is a total of 24 bits.
The number of decimal bits is chosen as the minimum number of bits
needed to represent the number in fixed-point.
One bit is reserved for the sign, and the rest are fractional bits.
During computation of filter outputs, the precision of the filter taps
is converted to the precision contained in the "TapsPrecision" parameter.
<p>
To load the filter coefficients from a file, simply replace the default
coefficients with the string "&lt;filename".
It is advisable to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the FIR filter to work as expected regardless of
the directory in which the Ptolemy process actually runs.
It is best to use tilde characters in the filename to reference them to
the user's home directory.
This way, future file system reorganizations will have minimal effect.
<p>
When the <i>decimation</i> (<i>interpolation</i>)
state is different from unity, the filter behaves exactly
as it were followed (preceded) by a DownSample (UpSample) star.
However, the implementation is much more efficient than
it would be using UpSample and DownSample stars because
a polyphase structure is used internally, thereby avoiding the
unnecessary memory locations and multiplication by zero.
Arbitrary sample rate conversions by rational factors can
be accomplished in this way.
<p>
To design a filter for a multirate system, simply assume that the
sample rate is the product of the interpolation parameter and
the input sample rate, or equivalently, the product of the decimation
parameter and the output sample rate.
<a name="multirate filter design"></a>
<a name="filter design, multirate"></a>
<a name="filter, multirate"></a>
In particular, considerable care must be taken to avoid aliasing.
Specifically, if the input sample rate is f,
then the filter stopband should begin before <i>f/</i>2.
If the interpolation ratio is <i>i</i>, then <i>f/</i>2 is a fraction 1<i>/</i>(2<i> i</i>)
of the sample rate at which you must design your filter.
<p>
The <i>decimationPhase</i> parameter is somewhat subtle.
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
<p>
For more information about polyphase filters, see [1-2].
<a name="Harris, F. J."></a>
<a name="Vaidyanathan, P. P."></a>
<h3>References</h3>
<p>[1]  
F. J. Harris,
``Multirate FIR Filters for Interpolating and Desampling'', in
<i>Handbook of Digital Signal Processing</i>, Academic Press, 1987.
<p>[2]  
P. P. Vaidyanathan,
``Multirate Digital Filters, Filter Banks, Polyphase
Networks, and Applications: A Tutorial'',
<i>Proc. of the IEEE</i>, vol. 78, no. 1, pp. 56-93, Jan. 1990.
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
        protected {
		Fix Accum, fixIn, out, tap;
                int phaseLength;
        }
        setup {
                SDFFix::setup();

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
                phaseLength = taps.size() / i;
                if ((taps.size() % i) != 0) phaseLength++;

		// Set the precision on the fixed-point variables
		fixIn = Fix( ((const char *) InputPrecision) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid InputPrecision" );

		tap = Fix( ((const char *) TapPrecision) );
		if ( tap.invalid() )
		  Error::abortRun( *this, "Invalid TapPrecision" );

		Accum = Fix( ((const char *) AccumulationPrecision) );
		if ( Accum.invalid() )
		  Error::abortRun( *this, "Invalid AccumulationPrecision" );

		out = Fix( ((const char *) OutputPrecision) );
		if ( out.invalid() )
		  Error::abortRun( *this, "Invalid OutputPrecision" );

		// Set the overflow handlers for assignments/computations
		fixIn.set_ovflow( ((const char *) OverflowHandler) );
		if ( fixIn.invalid() )
		  Error::abortRun( *this, "Invalid OverflowHandler" );
		tap.set_ovflow( ((const char *) OverflowHandler) );
		Accum.set_ovflow( ((const char *) OverflowHandler) );
		out.set_ovflow( ((const char *) OverflowHandler) );

		// Set all fixed-point assignments/computations to use rounding
		fixIn.set_rounding( int(RoundFix) );
		tap.set_rounding( int(RoundFix) );
		Accum.set_rounding( int(RoundFix) );
		out.set_rounding( int(RoundFix) );
        }
        go {
            int phase, tapsIndex;
            int Interp = interpolation;
            int Decim = decimation;
            int outCount = Interp - 1;

            // phase keeps track of which phase of filter coefficients are used
            // Starting phase depends on the decimationPhase state.
            phase = Decim - int(decimationPhase) - 1;

            // Interpolate once for each input consumed
            for (int inC = 1; inC <= Decim; inC++) {
                // Produce however many outputs are required
		// for each input consumed
                while (phase < Interp) {
                   Accum = 0.0;
                   // Compute the inner product.
                   for ( int i = 0; i < phaseLength; i++ ) {
			// We use a temporary variable to avoid gcc2.7.2/2.8 problems
			Fix tmpA = (signalIn%(Decim - inC + i)); 
                        tapsIndex = i * Interp + phase;
                        if (tapsIndex >= taps.size())
                            tap = 0.0;
                        else 
                            tap = taps[tapsIndex];
        
                        if ( int(ArrivingPrecision) )
                            Accum += tap * tmpA;
                        else {
                            fixIn = tmpA;
                            Accum += tap * fixIn;
			}
                   }
                   out = Accum;
                   // note: output%0 is the last output chronologically
                   signalOut%(outCount--) << out;
                   phase += Decim;
                }
                phase -= Interp;
            }
        }
        // A wrap-up method is inherited from SDFFix
        // If you defined your own, you should call SDFFix::wrapup()
}

defstar {
        name {FIRFix}
        domain {SDF}
        desc {
A Finite Impulse Response (FIR) filter with fixed-point capabilities.
The fixed-point coefficients are in the "taps" state variable.
Default coefficients give an 8th order, linear phase lowpass
filter. To read coefficients from a file, replace the default
coefficients with "<fileName".
        }
        version { $Id$ }
        author { E. A. Lee, A. Khazeni }
        copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { SDF dsp library }
        explanation {
.pp
This star implements a finite-impulse response filter with multirate capability
.
.Id "filter, FIR"
.Id "FIR filter"
The default coefficients correspond to an eighth order, equiripple,
linear-phase, lowpass filter.  The 3dB cutoff frequency at about 1/3
of the Nyquist frequency.  To load filter coefficients from a file,
simply replace the default coefficients with the string "<filename".
.pp
It is advisable to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the FIR filter to work as expected regardless of
the directory in which the ptolemy process actually runs.
It is best to use tilde's in the filename to reference them to user's
home directory.  This way, future filesystem reorganizations
will have minimal effect.
.pp
When the \fIdecimation\fP (\fIinterpolation\fP)
state is different from unity, the filter behaves exactly
as it were followed (preceded) by a DownSample (UpSample) star.
However, the implementation is much more efficient than
it would be using UpSample and DownSample stars;
a polyphase structure is used internally, avoiding unnecessary use
of memory and unnecessary multiplication by zero.
Arbitrary sample-rate conversions by rational factors can
be accomplished this way.
.pp
To design a filter for a multirate system, simply assume the
sample rate is the product of the interpolation parameter and
the input sample rate, or equivalently, the product of the decimation
parameter and the output sample rate.
.Ir "multirate filter design"
.Ir "filter design, multirate"
.Ir "filter, multirate"
In particular, considerable care must be taken to avoid aliasing.
Specifically, if the input sample rate is f,
then the filter stopband should begin before f/2.
If the interpolation ratio is i, then f/2 is a fraction 1/2i
of the sample rate at which you must design your filter.
.pp
The \fIdecimationPhase\fP parameter is somewhat subtle.
It is exactly equivalent the phase parameter of the DownSample star.
Its interpretation is as follows; when decimating,
samples are conceptually discarded (although a polyphase structure
does not actually compute the discarded samples).
If you are decimating by a factor of three, then you will select
one of every three outputs, with three possible phases.
When decimationPhase is zero (the default),
the latest (most recent) samples are the ones selected.
The decimationPhase must be strictly less than
the decimation ratio.
.pp
For more information about polyphase filters, see F. J. Harris,
"Multirate FIR Filters for Interpolating and Desampling", in
\fIHandbook of Digital Signal Processing\fR, Academic Press, 1987.
        }
        seealso { FIRCx, Biquad, UpSample, DownSample,
                  firDemo, interp, multirate }
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
                desc { Filter tap values. The precision of these taps
in bits by default is 1.24.  Currently if a different precision is desired
the cast to the new precision must be done explicitly in the go directive 
of the star as done here. }
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
Use the arriving particles as they are if YES.  Cast them to the
precision specified by the parameter "InputPrecision" otherwise. }
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "2.14" }
                desc {
Precision of the input in bits.  The input particles are only casted
to this precision if the parameter "ArrivingPrecision" is set to NO.}
        }
        defstate {
                name { TapPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the taps in bits. }
        }
        defstate {
                name { AccumulationPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the accumulation in bits. }
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the output in bits. } 
        }
        protected {
                const char* IP;
                const char* TP;
                const char* AP;
                const char* OP;
                int In_len;
                int In_intBits;
                int Tap_len;
                int Tap_intBits;
                int Accum_len;
                int Accum_intBits;
                int Out_len;
                int Out_intBits;
                int phaseLength;
        }
        setup {
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

                IP = InputPrecision;
                TP = TapPrecision;
                AP = AccumulationPrecision;
                OP = OutputPrecision;
                In_len = Fix::get_length (IP);
                In_intBits = Fix::get_intBits (IP);
                Tap_len = Fix::get_length (TP);
                Tap_intBits = Fix::get_intBits (TP);
                Accum_len = Fix::get_length (AP);
                Accum_intBits = Fix::get_intBits (AP);
                Out_len = Fix::get_length (OP);
                Out_intBits = Fix::get_intBits (OP);
        }
        go {
            int phase, tapsIndex;
            int Interp = interpolation;
            int Decim = decimation;
            int outCount = Interp-1;
            Fix out(Out_len, Out_intBits), tap(Tap_len, Tap_intBits);
            Fix Accum(Accum_len, Accum_intBits), fixIn(In_len, In_intBits);

            // phase keeps track of which phase of the filter coefficients are used.
            // Starting phase depends on the decimationPhase state.
            phase = Decim - int(decimationPhase) - 1;

            // Interpterate once for each input consumed
            for (int inC = 1; inC <= Decim; inC++) {
                // Produce however many outputs are required
                // for each input consumed
                while (phase < Interp) {
                   Accum = 0.0;
                   // Compute the inner product.
                   for (int i = 0; i < phaseLength; i++) {
                        tapsIndex = i * Interp + phase;
                        if (tapsIndex >= taps.size())
                            tap = 0.0;
                        else 
                            tap = taps[tapsIndex];
        
                        if(int(ArrivingPrecision)) 
                            Accum += tap * Fix(signalIn%(Decim - inC + i));
                        else {
                            fixIn =  Fix(signalIn%(Decim - inC + i));
                            Accum += tap * fixIn;}
                   }
                   out = Accum;
                   // note: output%0 is the last output chronologically
                   signalOut%(outCount--) << out;
                   phase += Decim;
                }
                phase -= Interp;
            }
        }
}


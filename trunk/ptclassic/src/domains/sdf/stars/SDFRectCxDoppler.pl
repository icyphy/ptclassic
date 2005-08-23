defstar {
	name { RectCxDoppler }
	derivedFrom { RectCx }
	domain { SDF }
	desc {
Generate a rectangular pulse of width "width".  If "period" is greater
than zero, then the pulse is repeated with the given period.  The height
of the pulse is a complex quantity that is a function of the Doppler
parameters.
	}
	version { @(#)SDFRectCxDoppler.pl	1.8	10/07/96 }
	author { Karim-Patrick Khiar }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
This pulse generator is similiar to the <i>Rect</i> star, except
that it is complex-valued and supports several additional options
for Doppler effects.  This star was initially intended for radar
simulations.

<p>

The magnitude of the height of the pulse is equal to:
<pre>
SqrPthn * 10 <sup>SNRn/20</sup>

</pre>
The phase of the height of the pulse is equal to:
<pre>
Pi(2.Fpor(count-sdelay)/Fsimu - 
bandwidth(count-sdelay)<sup>2</sup>/Te.Fsimu<sup>2</sup> + 
4.dopplercount.Fe.Tp.vn/c)
</pre>

	}
	location { SDF main library }
	defstate {
		name { bandwidth }
		type { float }
		default { 1.0e9 }
		desc { Bandwidth of the signal. }
	}
	defstate {
		name { Te }
		type { float }
		default { "30e-6" }
		desc { Duration time }
	}
	defstate {
		name { Fe }
		type { float }
		default { 3.0e9 }
		desc { emission frequency. }
	}
	defstate {
		name { Fsimu }
		type { float }
		default { 8.0e6  }
		desc { Simulation Frequency. }
	}
	defstate {
		name { vn }
		type { float }
		default { 150.0 }
		desc { Target velocity. }
	}
	defstate {
		name { Tp }
		type { float }
		default { "1.0e-3" }
		desc { Pulse period. }
	}
	defstate {
		name { Np }
		type { int }
		default { 16 }
		desc { Pulse number. }
	}
	defstate {
		name { Fpor }
		type { float }
		default { 3.0e9 }
		desc { Carrier frequency. }
	}
	defstate {
		name { c }
		type { float }
		default { 3.0e8 }
		desc { Light speed. }
	}
	defstate {
		name { SNRn }
		type { float }
		default { 10.0 }
		desc { Signal to noise ratio. }
	}
	defstate {
		name { SqrPthn }
		type { float }
		default { 1.0 }
		desc { Square root of Noise power. }
	}
	defstate {
		name { sdelay }
		type { float }
		default { 0 }
		desc { Delay for the target. }
	}
	defstate {
		name { dopplercount }
		type { int }
		default { 0 }
		desc { Internal state }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	hinclude { <math.h> }
	constructor {
		height.setAttributes(A_NONCONSTANT | A_NONSETTABLE);
	}
	protected {
		double arg1;
		double arg2;
		double doparg0;
		double amp;
		int dopplerChanged;
	}
	setup {
		count = 0;
		dopplercount = 0;
		dopplerChanged = TRUE;

		// k_minus_kd = k - kd
		double k_minus_kd = int(count) - double(sdelay);

		// Phase of pulse height is
		// arg = Pi * (arg1 + arg2 + doparg)

		// Define arg1 = B.(k-kd)^2/(Te.Fsimu^2)
		arg1 = k_minus_kd * k_minus_kd;
		arg1 *= double(bandwidth);
		arg1 /= double(Te) * double(Fsimu) * double(Fsimu);

		// Define arg2 = 2.fpor.(k-kd)
	        arg2 = 2.0 * double(Fpor) * k_minus_kd / double(Fsimu);

		// Define doppler effect doparg where
		// doparg = doparg0 * dopplercount / c
		doparg0 = 4.0 * double(vn) * double(Fe) * double(Tp);

		// Magnitude of pulse height
		amp = double(SqrPthn) * pow(10.0,(double(SNRn)/20.0));
	}
	go {
                if ( int(count) < int(width) ) {
		  // Recompute pulse height if the dopplerCount has changed
		  if ( dopplerChanged ) {
		    double doparg = doparg0 * double(dopplercount)/double(c);
	            double arg = M_PI * (arg1 + arg2 + doparg) ;
	            double real = amp * cos(arg);
	            double imag = amp * sin(arg);
		    height = Complex(real,imag);
		  }
                }

		SDFRectCx::go();

		dopplerChanged = int(period) > 0 && int(count) == 0;
		if ( dopplerChanged ) {
		  dopplercount = int(dopplercount) + 1;
		  if ( int(Np) > 0 && int(dopplercount) >= int(Np))
		    dopplercount = 0;
		}
	}
}

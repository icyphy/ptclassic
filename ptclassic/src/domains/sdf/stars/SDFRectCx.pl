defstar {
	name { RectCx }
	domain { SDF }
	desc {
Generate a rectangular pulse of width "width" (default 240).  If
"period" is greater than zero, then the pulse is repeated with the
given period.  The height of the pulse is a complex quantity.
The magnitude of the height, which is a function of the square root
of the noise power "SqrPthn" and the signal-to-noise ratio "SNRn",
is equal to SqrPthn * 10^(SNRn/20).  The phase of the height is
equal to Pi ( 2 Fpor (count - sdelay) / Fsimu - bandwidth (count - sdelay)^2 /
(Te Fsimu^2) + 4 dopplercount Fe Tp vn / c ).
	}
	version { $Id$ }
	author { Karim-Patrick Khiar }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	explanation {
This pulse generator is similiar to the \fIRect\fR star, except
that it is complex-valued and supports several additional options
for Doppler effects.  This star was initially intended for radar
simulations.
	}
	location { SDF main library }
	output {
		name { output }
		type { complex }
	}
        defstate {
		name { height }
		type { complex }
		default { "(1.0,0.0)" }
		desc { Heigth of the rectangular pulse. }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
        defstate {
		name { width }
		type { int }
		default { 240 }
		desc { Width of the rectangular pulse. }
	}
	defstate {
		name { period }
		type { int }
		default { 1024 }
		desc { If greater than zero, the period of the pulse stream. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }
		desc { Internal state }
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	defstate {
		name { bandwidth }
		type { float }
		default { 1.0e9 }
		desc { Bandwidth of the signal. }
	}
	defstate {
		name { Te }
		type { float }
		default { "30.0*10^-6"  }
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
		attributes { A_SETTABLE|A_NONCONSTANT }
	}
	hinclude { <math.h> }
	setup {
		count = 0;
		dopplercount = 0;

		double real = 0.0, imag = 0.0;
		double k_minus_kd = int(count) - double(sdelay);

		// Definition arg1= B.(k-kd)^2/(Te.Fsimu^2)
		double arg1 = k_minus_kd * k_minus_kd;
		arg1 *= double(bandwidth);
		arg1 /= double(Te) * double(Fsimu) * double(Fsimu);

		// Definition arg2 = 2.fpor.(k-kd)
	        double arg2 = 2.0 * double(Fpor) * k_minus_kd / double(Fsimu);

		// Definition doppler effect doparg
		double doparg = 4.0 * double(vn) * double(Fe) * double(Tp);
		doparg *= double(dopplercount)/double(c);

	        double arg = M_PI * (arg1 + arg2 + doparg) ;
		double amp = double(SqrPthn) * pow(10.0,(double(SNRn)/20.0));

	        real = amp * cos(arg);
	        imag = amp * sin(arg);
		height = Complex(real,imag);
	}
	go {
		Complex pulseHeight(0.0,0.0);

		if ( int(count) < int(width) ) {
		  pulseHeight = Complex(height);
		}

		output%0 << pulseHeight;

		count = int(count) + 1;
		if ( int(period) > 0 && int(count) >= int(period) ) {
		  count = 0;
		  dopplercount = int(dopplercount) + 1;
		  if ( int(Np) > 0 && int(dopplercount) >= int(Np))
		    dopplercount = 0;
		}
	}
}

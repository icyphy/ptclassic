defstar {
	name { RectCx }
	domain { SDF }
	desc {
Generate a rectangular pulse of height "height" (default 1.0).
and width "width" (default 8).  If "period" is greater than zero,
then the pulse is repeated with the given period.
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
that it is complex-valued and supports several addition options
for Doppler effects.
}
	location { SDF main library }
	output {
		name { output }
		type { complex }
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
		name { sdelay }
		type { float }
		default { 0 }
		desc { Delay for the target. }
	}
	defstate {
		name { count }
		type { int }
		default { 0 }	}
	defstate {
		name { dopplercount }
		type { int }
		default { 0 }	}
	hinclude { <math.h> }
	go {
		double r = 0.0;
		double i = 0.0;
	        double arg  = 0.0;
		double arg1 = 0.0;
		double arg2 = 0.0;
		double amp  = 0.0;
		double doparg = 0.0;

		if ( int(count) < int(width) ) {

		  // Definition arg1= B.(k-kd)^2/(Te.Fsimu^2)
	          arg1 = (double(count)-sdelay)*(double(count)-double(sdelay));
		  arg1 = double(bandwidth)*arg1;
		  arg1 = arg1/(double(Te)*double(Fsimu)*double(Fsimu));

		  // Definition arg2= 2.fpor.(k-kd)
	          arg2 =  2.0*double(Fpor)*(double(count)-double(sdelay));
	          arg2 = arg2/double(Fsimu);

		  // Definition doppler effect doparg
		  doparg =4.0*double(vn)*double(Fe)*double(Tp)/double(c);
		  doparg = doparg * double(dopplercount);

	          arg = M_PI * (arg1 + arg2 + doparg) ;

		  amp = double(SqrPthn) * pow(10.0,(double(SNRn)/20.0) );

	          r = amp * cos(arg);
	          i = amp * sin(arg);
		}
		output%0 << Complex( r, i);

		count = int(count) + 1;
		if ( int(period) > 0 && int(count) >= int(period) ) {
		  count = 0;
		  dopplercount = int(dopplercount) +1;
		  if ( int(Np) > 0 && int(dopplercount) >= int(Np))
		    dopplercount = 0;
		}
	}
}

defstar {
	name { RectCx }
	domain { SDF }
	desc {
Generate a rectangular pulse of height "height" (default 1.0).
and width "width" (default 240).  If "period" is greater than zero,
then the pulse is repeated with the given period.
	}
	version { @(#)SDFRectCx.pl	1.6	06 Oct 1996 }
	author { Brian L. Evans and Karim-Patrick Khiar }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
This pulse generator is similiar to the <i>Rect</i> star, except
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
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	hinclude { <math.h> }
	setup {
		count = 0;
	}
	go {
                Complex t(0.0,0.0);
                if (int(count) < int(width)) t = height;
                output%0 << t;
                count = int(count) + 1;
                if (int(period) > 0 && int(count) >= int(period)) count = 0;
	}
}

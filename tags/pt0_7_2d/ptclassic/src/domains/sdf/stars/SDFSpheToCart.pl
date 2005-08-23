defstar {
	name { SpheToCart }
	domain { SDF }
	desc { Convert magnitude and phase to rectangular form. }
	version { @(#)SDFSpheToCart.pl	1.5	07 Oct 1996 }
	author { Karim-Patrick Khiar }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
Compute the inner product of two vectors; one given by two angles in
spherical components, the other in cartesian components.
<pre>
&lt;R,U&gt; = [Rx, Ry, Rz]*[cos(sn)*sin(gn), sin(sn), cos(sn)*cos(gn)]'
</pre>
<a name="Compute the inner product of two vectors"></a>
<a name="format conversion, spherical to cartesian"></a>
	}
	location { SDF main library }
	output {
		name { x }
		type { float }
	}
	defstate {
		name { magnitude }
		type { float }
		default { 1.0 }
		desc { Magnitude of vector U. }
	}
	defstate {
		name { site  }
		type { float }
		default { 1.0 }
		desc {  elevation of vector U. }
	}
	defstate {
		name { gise }
		type { float }
		default { 1.0 }
		desc { azimuth of vector U. }
	}
	defstate {
		name { Rx }
		type { float }
		default { 1.0 }
		desc { First component of vector R. }
	}
	defstate {
		name { Ry }
		type { float }
		default { 1.0 }
		desc { Second component of vector R. }
	}
	defstate {
		name { Rz }
		type { float }
		default { 1.0 }
		desc { Third component of vector R. }
	}
	ccinclude { <math.h> }
	go {
// Line originally was:
// x%0 << magnitude * 
//        (cos(site)*sin(gise)*Rx + sin(site)*Ry+ cos(site)*cos(gise)*Rz) ;
// But sol2.cfront barfs with:
// line 63: Error: Overloading ambiguity between
// operator*(const Complex&, double) and FloatState::operator double().
//
// Now, we use casts whenever a state is referenced

	double sited = double(site);
	double gised = double(gise);

	x%0 << (double(magnitude) * (cos(sited)*sin(gised)*double(Rx) +
                                     sin(sited)*double(Ry) +
 				     cos(sited)*cos(gised)*double(Rz)));
	}
}

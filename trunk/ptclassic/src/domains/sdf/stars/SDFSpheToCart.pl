defstar {
	name { SpheToCart }
	domain { SDF }
	desc { Convert magnitude and phase to rectangular form. }
	version { $Id$ }
	author { Karim-Patrick Khiar }
	copyright {
Copyright (c) 1990-$Id$ The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	explanation {
Compute the inner product of two vectors; one given by two angles in spherical components, the other in cartesian components.
        <R,U> = [Rx, Ry, Rz]*[cos(sn)*sin(gn), sin(sn), cos(sn)*cos(gn)]'
.Id "Compute the inner product of two vectors"
.Id "format conversion, spherical to cartesian"
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
		x%0 << magnitude *(cos(site)*sin(gise)*Rx + sin(site)*Ry+ cos(site)*cos(gise)*Rz) ;
	}
}

defstar {
	name { ExpGen }
	domain { SDF } 
	desc { 	Complex Exponential Generator.}
	author { N. Becker }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { ~neal/example/fft }
	defstate {
	  name {freq}
	  type {float}
	  default {1}
	  desc {frequency}
	}
	defstate {
	  name {rate}
	  type {float}
	  default {1}
	  desc {sample rate}
	}
	output {
		name { output }
		type { complex }
	}
	private {
	  double t;
	  double dt;
	}
	setup {
	  t = 0;
	  dt = (double)freq/(double)rate * 2 * M_PI;
	}
	ccinclude { <math.h> }
	go {
		output%0 << Complex( cos( t ), sin( t ) );
		t += dt;
	}
}

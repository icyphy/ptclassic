defcore {
	name { IDCT }
	domain { ACS }
    	coreCategory { FPSim }
    	corona { IDCT } 
	desc { computes the inverse discrete cosine transform of a block of 8 samples }
	version { @(#)ACSIDCTFPSim.pl	1.2 09/08/99 }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
	ccinclude { <math.h> }
	protected {
		double c[8][8];
	}
	setup {
		int freq, time;
		double	scale;
		for(freq=0; freq < 8; freq++)
		{
			scale = (freq == 0) ? 1.0 : sqrt(2.0);
			for(time=0; time<8; time++)
				c[freq][time] = scale*cos((M_PI/8.0)*freq*(time + 0.5));
		}
		corona.input.setSDFParams(8,7);
		corona.output.setSDFParams(8,7);
	}
	go {
		int freq, time;
		double	partial_product;

		for( time=0; time<8; time++ ) {

			partial_product = 0.0;

			for (freq=0; freq<8; freq++ ) 
				partial_product += 
				c[freq][time]*double(corona.input%freq);

			corona.output%time << partial_product;
		}
	}
}
		
		
		

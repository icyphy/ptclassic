defstar {
	name { Variance }
	domain { SDF } 
	desc { Compute Mean and Variance }
	author { N. Becker }
	version { @(#)SDFVariance.pl	1.6	3/7/96 }
	location { SDF user contribution library }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	input {
	  name { in }
	  type { float }
	}
	defstate {
	  name { skip }
	  type { int }
	  default { 0 }
	  desc { number of tokens to ignore }
	}
	defstate {
	  name { block }
	  type { int }
	  default { 1 }
	  desc { process in blocks of this size }
	}
	output {
		name { mean }
		type { float }
	}
	output {
		name { variance }
		type { float }
	}
	private {
	  float SumXsqr;
	  float SumX;
	  int skipcount;
	  int count;
	}
	setup {
	  SumXsqr = 0;
	  SumX = 0;
	  count = 0;
	  skipcount = int(skip);
	  in.setSDFParams( int(block), int(block) - 1);
	}
	code {
	  inline double mySqr( double x ) {
	    return x * x;
	  }
	}
	go {
	  for( int i = int(block) - 1; i >= 0; i-- ) {
	    if( skipcount )
	      skipcount--;
	    else {
	      count++;
	      float input = (float)(in%i);
	      SumX += input;
	      SumXsqr += mySqr( input );
	    }
	  }
	  if( count ) {
	    mean%0 << (SumX / count);
	    variance%0 << ( (SumXsqr/count) - mySqr( SumX/count ) );
	  }
	  else {
// have to output something
	    mean%0 << 0;
	    variance%0 << 0;
	  }
	}
}

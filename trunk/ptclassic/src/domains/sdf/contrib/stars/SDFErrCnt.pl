defstar {
	name { ErrCnt }
	domain { SDF } 
	desc { Error Counter.}
	author { N. Becker }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF user contribution library }
	input {
	  name { a }
	  type { int }
	}
	input {
	  name { b }
	  type { int }
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
		name { bits }
		type { int }
	}
	output {
		name { errors }
		type { int }
	}
	private {
	  int numBits;
	  int numErrs;
	  int count;
	}
	setup {
	  numBits = 0;
	  numErrs = 0;
	  count = (int)skip;
	  a.setSDFParams( (int)block, (int)block - 1);
	  b.setSDFParams( (int)block, (int)block - 1);
	}
	go {
	  for( int i = (int)block - 1; i >= 0; i-- ) {
	    if( count )
	      count--;
	    else {
	      numBits++;
	      if( (int)(a%i) != (int)(b%i) )
	      numErrs++;
	    }
	  }
	  bits%0 << numBits;
	  errors%0 << numErrs;
	}
}

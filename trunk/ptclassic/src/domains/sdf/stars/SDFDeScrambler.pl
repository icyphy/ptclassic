defstar {
	name { DeScrambler }
	domain { SDF }
	desc {
Descramble the input bit sequence using a feedback shift register.
The taps of the feedback shift register are given by the "polynomial"
parameter.  This is a self-synchronizing descrambler that will exactly
reverse the operation of the Scrambler star if the polynomials are the same.
The low order bit of the polynomial should always be set. For more information,
see the documentation for the SDF Scrambler star and Lee and Messerschmitt,
Digital Communication, Second Edition, Kluwer Academic Publishers, 1994,
pp 595-603.
	}
	explanation {
.IE "feedback shift register"
.IE "pseudo-random sequence"
.IE "PN sequence"
.IE "primitive polynomial"
.IE "maximal length feedback shift register"
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
	  name { input }
	  type { int }
	  desc { bit sequence in (zero or nonzero) }
	}
	output {
	  name { output }
	  type { int }
	  desc { bit sequence out (zero or one) }
	}
	defstate {
	  name { polynomial }
	  type { int }
	  default { 0440001 }
	  desc { generator polynomial for the maximal length shift register }
	}
	defstate {
	  name { shiftReg }
	  type { int }
	  default { 0 }
	  desc { the shift register }
	}
	protected {
	  int mask;
	}
	setup {
	  // Should check that generator polynomial does not exceed 31 bits. How?
	  mask = int(polynomial);
	  // To avoid sign extension problems, the hob must be zero
	  if (mask < 0) {
	    Error::abortRun(*this,"Sorry, polynomials of order higher than 31 are not supported");
	    return;
	  }
	  if (!(mask & 1)) {
	    Error::warn(*this,"The low-order bit of the polynomial is not set. Input will have no effect");
	  }
	}
	go {
	  int reg = int(shiftReg) << 1;
	  // put the input in the low order bit
	  reg += (int(input%0) != 0);
	  int masked = mask & reg;
	  // Now we need to find the parity of "masked".
	  int parity = 0;
	  // Calculate the parity of the masked word.
	  while (masked > 0) {
	    parity = parity ^ (masked & 1);
	    masked = masked >> 1;
	  }
	  output%0 << parity;
	  shiftReg = reg;
	}
}

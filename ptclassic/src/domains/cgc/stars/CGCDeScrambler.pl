defstar {
	name { DeScrambler }
	domain { CGC }
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
Copyright (c) 1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
	setup {
	  // Should check that generator polynomial does not exceed 31 bits. How?
	  // To avoid sign extension problems, the hob must be zero
	  if (int(polynomial) < 0) {
	    Error::abortRun(*this,"Sorry, polynomials of order higher than 31 are not supported");
	    return;
	  }
	  if (!(int(polynomial) & 1)) {
	    Error::warn(*this,"The low-order bit of the polynomial is not set. Input will have no effect");
	  }
	}
	codeblock(descramble) {
	  int reg, masked, parity;
	  reg = $ref(shiftReg) << 1;
	  /* put the input in the low order bit */
	  reg += ($ref(input) != 0);
	  masked = $val(polynomial) & reg;
	  /* Now we need to find the parity of "masked". */
	  parity = 0;
	  /* Calculate the parity of the masked word */
	  while (masked > 0) {
	    parity = parity ^ (masked & 1);
	    masked = masked >> 1;
	  }
	  $ref(output) = parity;
	  $ref(shiftReg) = reg;
	}
	go {
	  addCode(descramble);
	}
}

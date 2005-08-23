defstar {
	name { Scrambler }
	domain { SDF }
	desc {
Scramble the input bit sequence using a feedback shift register.
The taps of the feedback shift register are given by the "polynomial"
parameter, which should be a positive integer.
The n-th bit of this integer indicates whether the n-th tap of the
delay line is fed back.
The low-order bit is called the 0-th bit, and should always be set.
The next low-order bit indicates whether the output of the first delay
should be fed back, etc.
The default "polynomial" is an octal number defining the V.22bis scrambler.
For an explanation of scramblers, see
Lee and Messerschmitt, Digital Communication, Second Edition,
Kluwer Academic Publishers, 1994, pp. 595-603.
	}
	htmldoc {
<a name="feedback shift register"></a>
<a name="pseudo-random sequence"></a>
<a name="pseudo-noise sequence"></a>
In scramblers based on feedback shift registers,
all the bits to be fed back are exclusive-ored
together (i.e., their parity is computed), and the result is exclusive-ored
with the input bit.
This result is produced at the output and shifted into the delay line.
With proper choice of polynomial, the resulting output appears highly random
even if the input is highly non-random (e.g., all zeros or all ones).
<p>
If the polynomial is a <i>primitive polynomial</i>, then the feedback shift
register is a so-called <i>maximal length feedback shift register</i>.
<a name="primitive polynomial"></a>
<a name="maximal length feedback shift register"></a>
This means that with a constant input, the output will be sequence
with period 2<sup><i>N</i></sup>-1, where <i>N</i> is the order 
of the polynomial (the length of the shift register).
This is the longest possible sequence.
Moreover, within this period, the sequence will appear to be white,
in that a computed autocorrelation will be very nearly an impulse.
Thus, the scrambler with a constant input can be very effectively used
to generate a pseudo-random bit sequence.
<p>
The maximal-length feedback shift register with constant input will
pass through 2<sup><i>N</i></sup>-1 states before returning to a state 
it has been in before.  This is one short of the 2<sup><i>N</i></sup> 
states that a register with <i>N</i> bits can take on.  This one missing 
state, in fact, is a <i>lock-up</i>
state, in that if the input is an appropriate constant, the scrambler will
cease to produce random-looking output, and will output a constant.
For example, if the input is all zeros, and the initial state of the
scrambler is zero, then the outputs will be all zero, hardly random.
This is easily avoided by initializing the scrambler to some non-zero state.
That is why the default value for the <i>shiftReg</i> is set to 1.
<p>
The <i>polynomial</i> must be carefully chosen. It must represent a
<i>primitive polynomial</i>, which is one that cannot be factored into two
(nontrivial) polynomials with binary coefficients.  See Lee and Messerschmitt
for more details.  For convenience, we give here a set of primitive polynomials
(expressed as octal numbers so that they are easily translated into taps
on shift register).  All of these will result in maximal-length pseudo-random
sequences if the input is constant and lock-up is avoided:
<pre>
order    polynomial
2        07
3        013
4        023
5        045
6        0103
7        0211
8        0435
9        01021
10       02011
11       04005
12       010123
13       020033
14       042103
15       0100003
16       0210013
17       0400011
18       01000201
19       02000047
20       04000011
21       010000005
22       020000003
23       040000041
24       0100000207
25       0200000011
26       0400000107
27       01000000047
28       02000000011
29       04000000005
30       010040000007
</pre>
<p>
The leading zero in the polynomial indicates an octal number.
Note also that reversing the order of the bits in any of these numbers
will also result in a primitive polynomial.
Thus, the default value for the polynomial parameter
is 0440001 in octal, or "100 100 000 000 000 001" in binary.
Reversing these bits we get "100 000 000 000 001 001" in binary, or
0400011 in octal.
This latter number is the one listed above as the primitive polynomial
of order 17.
The order is simply the index the highest-order non-zero in the polynomial,
where the low-order bit has index zero.
<p>
Since the polynomial and the feedback shift register are both implemented
using type "int", the order of the polynomial is limited by the size of
the "int" data type. 
For simplicity and portability, the polynomial is also not allowed
to be interpreted as a negative integer, so the sign bit cannot be used.
Thus, if "int" is a 32-bit word, then the highest order polynomial allowed is
30 (recall that indexing for the order starts at zero, and we cannot use
the sign bit).
Since many machines today have 32-bit integers, we give the primitive
polynomials above only up to order 30.
<h3>References</h3>
<p>[1]  
Edward Lee and David Messerschmitt, <i>Digital Communication</i>,
Second Edition, Kluwer Academic Publishers, 1994, pp. 595-603.
	}
	version { @(#)SDFScrambler.pl	1.9	10/08/96 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
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
	  default { 1 }
	  desc { the shift register }
	}
	protected {
	  int mask;
	}
	setup {
	  // The user interface should check that generator polynomial
	  // specification does not exceed the integer precision of the machine
	  mask = int(polynomial);
	  // To avoid sign extension problems, the high-order bit must be zero
	  if (mask < 0) {
	    Error::abortRun(*this,
			"Sorry, the polynomial must be a positive integer.");
	    return;
	  }
	  if (!(mask & 1)) {
	    Error::warn(*this,
			"The low-order bit of the polynomial is not set.",
			"Input will have no effect on the shift register.");
	  }
	}
	go {
	  int reg = int(shiftReg) << 1;
	  int masked = mask & reg;
	  // Now we need to find the parity of "masked"
	  int parity = 0;
	  // Calculate the parity of the masked word
	  while (masked > 0) {
	    // toggle parity if the low-order bit is one
	    parity = parity ^ (masked & 1);
	    masked = masked >> 1;
	  }
	  // Exclusive-or with the input
	  //   nonzero input: exclusive or with 0x01
	  //   zero input: no exclusive or
	  if ( int(input%0) ) {
	    parity = parity ^ 0x01;
	  }
	  output%0 << parity;
	  // Put the parity bit into the shift register
	  shiftReg = reg | parity;
	}
}

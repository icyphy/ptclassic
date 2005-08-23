defstar {
	name { DTMFPostTest }
	domain { SDF }
	version { @(#)SDFDTMFPostTest.pl	1.6	06 Oct 1996 }
	desc {
Returns whether or not a valid dual-tone modulated-frequency has
been correctly detected based on the last three detection results.
	}
	htmldoc {
The assumption is that the 100 msec DTMF interval has been split into
roughly four parts.  This star looks at the last three detection results,
which are represented as integers.  A new digit has been detected
if two consecutive detected digits are the same followed by a third
detected digit that is different.
<p>
This test is useful for two reasons.
First, it filters redundant hits so that only one is reported.
Second, it improves robustness against noisy DTMF signals and speech input.
<h3>References</h3>
<p>[1]  
Pat Mock, "Add DTMF Generation and Decoding to DSP-uP Designs,"
Electronic Data News, March 21, 1985.  Reprinted in
<i>Digital Signal Processing Applications with the TMS320 Family</i>,
Texas Instruments, 1986.
	}
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name{input}
		type{int}
	}
	input {
		name{valid}
		type{int}
	}
	output {
		name{output}
		type{int}
	}
	defstate {
		name { initialLastInput }
		type { int }
		default { "-1" }
		desc {
The value to assign to last and secondToLast internal states, which hold the
previous two valid inputs.  For the purposes of DTMF detection, it is
set to an integer that does not represent a DTMF digit.
		}
	}
	defstate {
		name { last }
		type { int }
		default { "-1" }
		desc {
internal state to store the last valid integer
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { secondToLast }
		type { int }
		default { "-1" }
		desc {
internal state to store the second-to-last valid integer
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	setup {
		last = int(initialLastInput);
		secondToLast = int(initialLastInput);
	}
	go {
		// If the current input is valid, then compare it
		// with the last and second-to-last valid inputs;
		// otherwise, return FALSE
		int retval = FALSE;
		int inputValue = int(initialLastInput);
		int lastValue = int(last);
		if ( int(valid%0) ) {
		  inputValue = int(input%0);
		  retval = ( inputValue == lastValue &&
		             lastValue != int(secondToLast) );
		}
		output%0 << retval;

		// Update the last and second-to-last input storage
		secondToLast = lastValue;
		last = inputValue;
	}
}

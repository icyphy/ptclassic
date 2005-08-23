defstar {
	name { Convolve }
	domain {VHDLB}
        desc {
Convolve two causal finite sequences.
Set truncationDepth larger than the number of output samples of interest.
        }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
This star convolves two causal finite input sequences.
In the current implementation, you should set the truncation
depth larger than the number of output samples of interest.
If it is smaller, you will get unexpected results after truncationDepth
samples.

A sample of the input is taken at every rising edge of the input synchroni-
zation clock.  The arrays storing the past input samples are flushed
(initialized to zero) only at the beginning of the execution of the star or
after "truncationDepth" samples have been sent to the output port.  Otherwise
at every rising clock edge, the contents of the arrays are shifted back by one
element to store the most recent samples at the beginning of the arrays.
Then, the contents of the two arrays are used in the calculation of the convo-
lution sum at every sample interval.
        }
	input {
		name { clock }
		type { int }
	}
	input {
		name { inputa }
		type { float }
	}
        input {
                name { inputb }
                type { float }
        }
	output {
		name { output }
		type { float }
	}
	state {
		name { truncationDepth }
		type { int }
		default { 256 }
		desc { Maximum number of terms in convolution sum. }
	}
	go {
	}
}

defstar {
	name { NumToBus }
	domain { SDF }
	desc {
This star accepts an integer and outputs the low-order bits that
make up the integer on a number of outputs, one bit per output.
The number of outputs should not exceed the wordsize of an integer.
	}
	author { Asawaree Kalavade }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { @(#)SDFNumToBus.pl	1.15	2/12/96 }
	input {
		name {input}
		type {int}
	}
	outmulti {
		name {output}
		type {int}
	}
	protected {
		int bits;
		int *binary;
	}
        constructor {
		bits = 0;
                binary = 0;
        }
        destructor {
                delete [] binary;
        }
        setup {
                bits = output.numberPorts();
                delete [] binary;
                binary = new int[bits];
        }
	go {
	    	int in = int(input%0);

		// convert the integer to binary and store it into an array
		// to be output on the bus, msb is the sign bit

		if (in < 0) {
			binary[bits-1] = 1;
			in += 2 << (bits-1);
		}
		else {
			binary[bits-1] = 0;
		}

		for (int i = 0; i < bits - 1; i++) {
 	     		binary[i] = in & 0x1;
			in >>= 1;
		}

		MPHIter nextp(output);
		PortHole* p;
		int j = 0;
		while ((p = nextp++) != 0) {
			(*p)%0 << binary[j++];
		}
	    } //go
}

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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
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
                binary = 0;
        }
        destructor {
                LOG_DEL; delete [] binary;
        }
        setup {
                bits = output.numberPorts();
                LOG_DEL; delete [] binary;
                LOG_NEW; binary = new int[bits];
        }
	go {
		int i =0;
	    	int in = int (input%0);

/* 	convert the integer to binary and store it into an array
	to be output on the bus, msb is the sign bit
*/

		if(in<0)	{binary[bits-1] = 1; in += 2 << (bits-1);}
		else		binary[bits-1] = 0;

		while (i< (bits-1)) 
		{
 	     		binary[i++] = in & 0x1;
			in = in >> 1;
		}

		MPHIter nextp(output);
		i=0;
		PortHole* p;
		while ((p=nextp++) != 0)
		{
			(*p)%0  << binary[i];
			i++;
		}

	    } //go
}

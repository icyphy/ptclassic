defstar {
	name { BusToNum }
	domain { SDF }
	desc {
This star accepts a number of input bit streams, where this number should not
exceed the wordsize of an integer.  Each bit stream has integer particles with
values 0, 3, or anything else.  These are interpreted as binary 0, tri-state,
or 1, respectively.  When the star fires, it reads one input bit from each 
input.
If any of the input bits is tri-stated, the output will be the previous output
(or the initial value of the "previous" parameter if the firing is the first 
one).
Otherwise, the bits are assembled into an integer word, assuming two's 
complement encoding, and sign extended.  The resulting signed integer is sent 
to the output.
	}
	author { Asawaree Kalavade }
	version { $Id$ }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	inmulti {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	defstate {
		name {previous}
		type {int}
		default {"0"}
		desc {previous value of output}
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
		bits = input.numberPorts();	
		LOG_DEL; delete [] binary;
		LOG_NEW; binary = new int[bits];
	}	
	go {
		MPHIter nexti(input);
		int number;

		for( int i=0; i< bits; i++){
			binary[i]= (*nexti++)%0;
		}

		for(i=0; i< bits; i++)
		{
	// The following is a Thor-specific condition check where we do not
	// want the number to be latched in if the bus is tristate (value 3)
			if(binary[i] ==3)
			{
				output%0 << int(previous); 
				return; 
			}
		}

		number = binary[bits-1] ? -1 : 0;

		for( i=(bits-2); i>=0; i--)
		{
			number <<= 1;
			int d = binary[i] ? 1 : 0;
			number += d;
		}

		previous= number;
		output%0 << number;	

	} // go
}


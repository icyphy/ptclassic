defstar {
	name { BusToNum }
	domain { SDF }
	desc {
This star accepts a number of input bit streams, where this number should not
exceed the wordsize of an integer.  Each bit stream has integer particles with
values 0, 3, or anything else.  These are interpreted as binary 0, tri-state,
or 1, respectively.  When the star fires, it reads one input bit from each input.
If any of the input bits is tri-stated, the output will be the previous output
(or the initial value of the "previous" parameter if the firing is the first one).
Otherwise, the bits are assembled into an integer word, assuming two's complement
encoding, and sign extended.  The resulting signed integer is sent to the output.
	}
	author { Asawaree Kalavade }
	version { $Id$ }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
		name {bits}
		type {int}
		default {"7"}
		desc {number of bits in input}
	}
	defstate {
		name {previous}
		type {int}
		default {"0"}
		desc {previous value of output}
	}
	
	go {
		int b;
		LOG_NEW; int* binary = new int[bits+1];
		MPHIter nexti(input);
		int number 	= 0;

	// latch bits into array
		for( int i=0; i< bits; i++){
			binary[i]= (*nexti++)%0;
		}

	// The following is a Thor-specific condition check where we do not
	// want the number to be latched in if the bus is tristate (value 3)
		for(i=0; i< bits; i++)
		{
			if(binary[i] ==3){
				output%0 << int(previous); 
				LOG_DEL; delete [] binary;
				return; 
			}
		}

	// check the MSB for sign bit and convert the number appropriately
        if(binary[bits-1]==1)
	{
                for(int j=0;j<(bits-1);j++)
		{
                	b=binary[j];
                        if(b==0) b=1;
                        else b=0;
                	binary[j]= b;
		}
                for(i=0;i<(bits-1);i++) 
		{
			switch( binary[(bits-2)-i]) 
			{
                        case    0:
                       		number <<=1;
                                break;
                        case    1:
                                number <<=1;
                                number +=1;
                                break;
                        }
		}
                number +=1;
                number = number * (-1);
	}
        else 
	{
		for(i=0;i<(bits-1);i++) 
		{
			switch( binary[(bits-2)-i]) 
			{
			case    0:
				number <<=1;
				break;
			case    1:
				number <<=1;
				number +=1;
				break;
			}
		}
         }

		previous= number;
		output%0 << number;	
		LOG_DEL; delete [] binary;
	} // go
}

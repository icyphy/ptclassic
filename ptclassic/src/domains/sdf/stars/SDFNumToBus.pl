defstar {
	name { NumToBus }
	domain { SDF }
	desc {
Receive as its input an integer corresponding to the bus contents and it
converts it to a bus with the required number of lines (which is read in
as a parameter).
	}
	author { Asawaree Kalavade }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
	defstate {
		name {bits}
		type {int}
		default {"7"}
		desc {number of bits to output}
	}
	go {
		int i =0 ;
		LOG_NEW; int *binary = new int[bits+1];
	
	    	int in = int (input%0);

/* 	convert the integer to binary and store it into an array
	to be output on the bus, msb is the sign bit
*/

	if(in<0){
	  binary[bits-1]=1;
	// in = in + 2^bits
	  in= ( 2 << (bits-1) ) + in;
		while (i< (bits-1)) {
 	     		binary[i++] = in & 0x1;
			in = in >> 1;
				    }
		}
	else {
	  binary[bits-1]=0;
		while (i< (bits-1) ) {
 	     		binary[i++] = in & 0x1;
			in = in >> 1;
					}
	     }
		MPHIter nextp(output);
		i=0;
		PortHole* p;
		while ((p=nextp++) != 0){
			(*p)%0  << binary[i];
			i++;
					}
		LOG_DEL; delete [] binary;
	    }
}

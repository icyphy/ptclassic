defstar {
	name { NumToBus }
	domain { SDF }
	desc {
This will receive as its input, as an integer corresponding to the bus 
contents and it converts it to a bus with the required no. of lines
(which is read in as a parameter).
	}
	author { Asawaree Kalavade }
	copyright { 1991 The Regents of the University of California }
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
	start {
	      }
	
	go {
		int i =0 ;
		int binary[bits+1];
	
	    	int in = int (input%0);

/* 	convert the integer to binary and store it into an array
	to be output on the bus, msb is the sign bit
*/

	if(in<0){
	  binary[bits-1]=1;
	// in = in + 2^bits
	  in= 128 + in;
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
	    }
}

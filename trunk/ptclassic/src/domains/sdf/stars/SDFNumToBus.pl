defstar {
	name { NumToBus }
	domain { SDF }
	desc {
This will receive as its input, as an integer, the bus contents
and then it will convert it to a bus with the required no. of lines.
(which is read in as a parameter ).
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
		int binary[8];
	
	    	int in = int (input%0);

/* 	convert the integer to binary and store it into an array
	to be output on the bus
*/

// fprintf(stderr,"from Out : input integer is : %d \n",in);
	if(in<0){
	  binary[6]=1;
	  in= 128 + in;
		while (i< 6) {
 	     		binary[i++] = in & 0x1;
			in = in >> 1;
					}
		}
	else {
	  binary[6]=0;
		while (i< 6 ) {
		// while (i< int (bits) ) 
 	     		binary[i++] = in & 0x1;
			in = in >> 1;
					}
	     }
//lsb not printed bec 1-6 match with 2-7 of thorParToSer
fprintf(stderr,"from Out: (6-0) "); 
			for(int c=6;c>=0;c--)
			fprintf(stderr,"%d ",binary[c]);
			fprintf(stderr,"\n");
/*	Output the array to the bits on the output port */
		MPHIter nextp(output);
		i=0;
		PortHole* p;
		while ((p=nextp++) != 0){
			(*p)%0  << binary[i];
	//		fprintf(stderr,"%d ",binary[i]);
			i++;
					}
	//fprintf(stderr,"\n");
/*
// lsb is bit 0
	printf("/n");
*/	
	    }
}

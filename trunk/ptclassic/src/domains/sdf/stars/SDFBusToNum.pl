defstar {
	name { BusToNum }
	domain { SDF }
	desc {
This will receive as its input,bus (with 'bits' no of lines ) 
and then convert it to an integer corresponding to the
value of this binary number and output that as a particle.
	}
	author { Asawaree Kalavade }
	version { $Id$ }
	copyright { 1991 The Regents of the University of California }
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
		desc {number of bits to output}
	}
	defstate {
		name {previous}
		type {int}
		default {"0"}
		desc {previous value of output}
	}
	
	go {
		int b;
		int number =0 ;
		int binary[8];
		int change=0;
		MPHIter nexti(input);

		for( int i=0; i< int (bits); i++){
		binary[i]= (*nexti++)%0;
		}
		for(i =0; i<7; i++)
		{
		if(binary[i] ==3){
		change=1;
		output%0 << int(previous); 
		return; }
		}

        if(binary[6]==1){
                for(int j=0;j<6;j++){
                b=binary[j];
                        if(b==0) b=1;
                        else b=0;
                binary[j]= b;}
                        for(i=0;i<6;i++) {
                            switch( binary[5-i]) {
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
        else {
                        for(i=0;i<6;i++) {
                            switch( binary[5-i]) {
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
}
}

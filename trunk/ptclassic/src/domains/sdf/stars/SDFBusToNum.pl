defstar {
	name { BusToNum }
	domain { SDF }
	desc {
This will receive as its input, a bus (with 'bits' no of lines ) 
and then output a signed integer corresponding to the value of this 
binary number.
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
		int binary[bits+1];
		MPHIter nexti(input);
		int number 	= 0;

	// latch bits into array
		for( int i=0; i< bits; i++){
			binary[i]= (*nexti++)%0;
		}

	// The following is a thor-specific condition check where we dont
	// want the number to be latched in if the bus is tristate (value 3)
		for(i=0; i< bits; i++)
		{
			if(binary[i] ==3){
				output%0 << previous; 
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

	} // go
}

defstar {
	name { BusToNum }
	domain { SDF }
	desc {
This will receive as its input,bus (with 'bits' no of lines ) 
and then convert it to an integer corresponding to the
value of this binary number and output that as a particle.
it is translating 7bits only
it disregards the lsb from the codec that is lost at ThorSerToPar
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
fprintf(stderr,"input bus: msb..lsb\t");		
		MPHIter nexti(input);
// fetch the bus contents
		for( int i=0; i< int (bits); i++){
		binary[i]= (*nexti++)%0;
		//fprintf(stderr,"%d",binary[i]);
		}
for(int l=6;l>=0;l--)
fprintf(stderr,"%d",binary[l]);
//		fprintf(stderr,"\n");	
/*	This is necessary as a temporary solution to a Thor limitation.
	The star receives the input from ThorSerToPar that outputs a '3'
	between two successful outputs of valid bits so that the
	sdf star detects a new particle being created for firing purposes.
	If the value received is a '3' however, we dont want it to be
	converted to an integer. So we put this check to exit under 
	such a case and thenn output the previous integer again.
	(precisely why we use a skipOne star in the demo following this
	to avoid duplicate samples being used).
*/
		for(i =0; i<7; i++)
		{
		if(binary[i] ==3){
		change=1;
		output%0 << int(previous); 
//		printf("returning\n");
		return; }
		}
/* this is being added for the sake of negative number conversion
   this looks at the msb and if one, then it will convert
*/

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

//fprintf(stderr,"after : %d \n",number);
//fprintf(stderr,"converted number is : %d\n ", number);
		previous= number;
		output%0 << number;	
}
}

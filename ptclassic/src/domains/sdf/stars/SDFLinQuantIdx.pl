defstar {
	name { NewQuant }
	domain { SDF }
	desc {
Quantizes input to one of N+1 possible output values using N thresholds.
For an input less than or equal to the n-th threshold, but larger than 
all previous thresholds, the output will be the n-th level.  
If the input is greater than all thresholds, the output is the N+1-th level.
There must be one more level than thresholds.
It asks for the number f values to be quantized to (255) and also
the range of values to quantize within. It will compute the step of
the quantizer.
Besides the quantized value being output, it will also output the level
number that it corresponds to for the purpose of converison to an
integer and then output serially to the Thor domain stars.
	}
	author { Asawaree }
	copyright { 1991 The Regents of the University of California }
	input {
		name {input}
		type {float}
	}
	output {
		name { amplitude }
		type {float}
	}
	output {
		name { stepNumber }
		type {int}
	}
	defstate {
		name {levels}
		type {int}
		default {"128"}
		desc {number of levels to quantize to }
	}
	defstate {
		name {low}
		type {float}
		default {"-3.0"}
		desc {lower limit of signal excursion }
	}
	defstate {
		name {high}
		type {float}
		default {"3.0"}
		desc {upper limit of signal excursion }
	}
	protected {
		float height;
		float thresholds[255];
		float values[255];
		int number;
	}
	start {
		int i;
		
		height= (double(high)-double(low)) /(int(levels)-1);
		thresholds[0]=double(low)+double(height);
		values[0]=double(low);

	for(i=1;i<int(levels);i++) {
	values[i]=values[i-1]+double(height);
	thresholds[i]=thresholds[i-1]+double(height);
			      }

	number= int ((-1)*( int(levels))* (0.5));
		}
	
	go {
		int i;
		

	    number= int ((-1)*( int(levels))* (0.5));

	    float in = float(input%0);
	    for(i = 0; i < int (levels); i++) {
		// if( in <= thresholds[i] ) 
		if( in < thresholds[i] ) {
		    amplitude%0 << values[i];
		    // stepNumber%0 << i;
		    stepNumber%0 << int(number);
		    return;
		}
		number++;
	//	output%0 << values[levels-1];
	//	output%0 << values[thresholds.size()];
        //	output%0 = values[levels-1];
	    }
	}
}

defstar {
	name { LinQuantIdx }
	domain { SDF }
	desc {
The input is quantized to "levels" number of levels for a specified
signal ranging between "low" and "high". On the "amplitude" port, the
quantized output is generated, while the "stepNumber" output is the
quantization level. This integer output is particularly needed for
the Thor stars that need an integer input.
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
	setup {
		int i;
		
		height		= (high-low) /(levels-1);
		thresholds[0]	= low+height;
		values[0]	= low;

		for(i=1;i<int(levels);i++) 
		{
			values[i]=values[i-1]+height;
			thresholds[i]=thresholds[i-1]+height;
		}

		number= int( (-1)*(int(levels))* (0.5));
		}
	
	go {
	    int i;
	    number= int( (-1)*(int(levels))* (0.5));

	    float in = input%0;

	    if( in >= high) {
		    amplitude%0 << double(high);
                    stepNumber%0 << (-1*number) -1 ;
		    return;
		}
	
	    for(i = 0; i < levels; i++) {
		if( in < thresholds[i] ) {
		    amplitude%0 << values[i];
		    stepNumber%0 << number;
		    return;
		}
		number++;
	    }
	}
}

defstar {
	name { LinQuantIdx }
	domain { SDF }
	desc {
The input is quantized to the number of levels given by the "levels" parameter.
The quantization levels are uniformly spaced between "low" and "high" inclusive.
Rounding down is performed, so that output level will equal "high" only if the input
level equals or exceeds "high". If the input is below "low", then the quantized
output will equal "low". The quantized value is output to the "amplitude" port,
while the index of the quantization level is output to the "stepNumber" port.
This integer output is useful for stars that need an integer input, such as Thor stars.
	}
	author { Asawaree Kalavade }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
		int number;
	}
	setup {
		if(high > low) 		
			height	= (high-low)/(levels-0);
		else			
		{
		Error::abortRun("quantization range incorrectly specified");
		return;
		}
	}
	go {
	    	float in = input%0;

	    	if( in >= high) 	
		{
			amplitude%0 << double(high);
                    	stepNumber%0 << int(levels);
		    	return;
		}
		else if(in <=low) 
		{
			amplitude%0 << double(low);
                    	stepNumber%0 << 0;
		    	return;
		}
		else
		{
			int step = int((in-low)/height);
			stepNumber%0 << step;
        		amplitude%0 << (double(low + step*height));
		}
	}
}

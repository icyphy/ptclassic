defstar {
    	name { Dummy Speech Compression }
	domain { SDF }
        version { $Id$ }
        author { Mudit Goel }
	copyright{}
        location { my smproj dir }
	desc { Dummy compression. Passes input as it is and gives out psuedo values as memory and power comsumed. }
	input {
            	name { input }
                type { ANYTYPE }
	output {
  		name { output }
		type { =input } 
        }
	output {
		name { Memory }
 		type { float }
        }
	output {
		name { Power }
		type { float }
        }
	defstate {
		name {Memory_consumed}
		type {float}
		default {"0.5"}
		desc {Fraction of memory consumed}   
        }
	defstate {
		name {Power_consumed}
		type {float}
		default {"0.5"}
		desc {Fraction of power consumed}
        }

	setup {
		if (float(Memory_Consumed) < 0 || float(Memory_consumed) > 1 ) {
                    Error::abortRun(*this, "Memory_consumed should be between 0 and 1");
                }
		else if (float(Power_consumed) > 1 || float(Power_consumed) < 0 ) {
                    Error::abortRun(*this, "Power_consumed should be between 0 and 1");
                }
        }	

 	go {
             output%0 << input;

         }

	wrapup {
	
             Memory%0 << Memory_consumed;

             Power%0 << Power_consumed;
         }
     }
		            

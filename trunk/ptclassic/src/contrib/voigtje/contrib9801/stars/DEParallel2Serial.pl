defstar {
	name { Parallel2Serial }
	domain { DE }
	version { @(#)DEParallel2Serial.pl	1.4 06/24/97 }
	author { J. Voigt }
	copyright { copyright (c) 1997 Dresden University of Technology,
                    Mobile Communications Systems 
        }
	location { The WiNeS-Library }
	desc { This is a parallel-to-serial converter. Data with the same time 
        stamp are read in parallel and put in a list. Each new value overrides 
        the older one in the list. Thus, the list always contains the very last
        inputs. On demand the current contents of the list is given out serial.
        }
	input {
	    name{ x_in }
	    type{ float }
	    desc{ input_values_x }
	}
	input {
	    name{ y_in }
	    type{ float }
	    desc{ input_values_y }
	}
	input {
	    name { demand }
	    type { anytype }
	    desc { at this time the star fires its output }
	}
	output {
	    name{ x_out }
	    type{ float }
	    desc{ output_values_x }
	}
	output {
	    name{ y_out }
	    type{ float }
	    desc{ output_values_y }
	}
	private {
	    float listex[21], listey[21];
	    int i, j;
	}
	constructor {
	    x_in.triggers();
	    y_in.triggers();
	}
	setup {
                for (int o = 1; o <= 20; o++) {
		    listex[o] = 0.0;
		    listey[o] = 0.0;
		} 
		i = 0; j = 0; 
        }
	go {
	    completionTime = arrivalTime;
	    while (x_in.dataNew) {
		i++;
		listex[i] = x_in.get();
		x_in.getSimulEvent();
	    }
	    while (y_in.dataNew) {
		j++;
		listey[j] = y_in.get();
		y_in.getSimulEvent();
	    } 
	    i = 0; j = 0;
	    if (demand.dataNew) {
		if (arrivalTime > 1.0) {
		    for (int o = 1; o <= 20; o++) {
			x_out.put(completionTime + i * 0.01) << listex[o];
			y_out.put(completionTime + i * 0.01) << listey[o];
		    }
		}
                // reset the list
		demand.dataNew = FALSE;
	    } 
	}
    }

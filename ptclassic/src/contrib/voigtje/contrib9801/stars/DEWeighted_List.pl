defstar {
	name { Weighted_List }
	domain { DE }
	version { @(#)DEWeighted_List.pl	1.1 06/24/97 }
	author { J. Voigt }
	copyright { copyright (c) 1997 Dresden University of Technology,
                    Mobile Communications Systems 
        }
	location { The WiNeS-Library }
	desc { An input value is read in. Depending on its value one or more 
        counters in a list are increased. The outputs are the contents of the 
        counters, weighted by the number of all incoming values. Thus, the
        probability that an incoming value is smaller than a specific value is 
        given out for all values in the list. Note that all outputs have the 
        same timestamp.
        }
	input {
            name{ CIR }
            type{ float }
            desc{ CIR-value for one time slot }
	}
	output {
            name{ testx }
            type{ float }
            desc{ outage probabilities }
        }
	output {
            name{ testy }
            type{ float }
            desc{ outage probabilities }
	}
	private {
            float value; 
	    int number, how_many_outputs;
	    float dummy;
	    int liste[21];
	}
	setup { 
            number = 0;
            how_many_outputs = 20;
            for (int i = 1; i <= how_many_outputs; i++) liste[i] = 0;
        }
	go {
            completionTime = arrivalTime;
            value = CIR.get();
            if (value < 5.0) liste[1]++;
            if (value < 10.0) liste[2]++;
            if (value < 15.0) liste[3]++;
            if (value < 20.0) liste[4]++;
            if (value < 25.0) liste[5]++;
            if (value < 30.0) liste[6]++;
            if (value < 35.0) liste[7]++;
            if (value < 40.0) liste[8]++;
            if (value < 45.0) liste[9]++;
            if (value < 50.0) liste[10]++;
            if (value < 55.0) liste[11]++;
            if (value < 60.0) liste[12]++;
            if (value < 65.0) liste[13]++;
            if (value < 70.0) liste[14]++;
            if (value < 75.0) liste[15]++;
            if (value < 80.0) liste[16]++;
            if (value < 85.0) liste[17]++;
            if (value < 90.0) liste[18]++;
            if (value < 95.0) liste[19]++;
            if (value < 100.0) liste[20]++;
            number++;
            for (int i = 1; i <= 20; i++) {
                dummy = (float) ((float) liste[i] / (float) number);
                dummy *= 100.0;
                testx.put(completionTime) << (float)(5*i);
                testy.put(completionTime) << (float)(dummy);
            }; 
        }
    }
    

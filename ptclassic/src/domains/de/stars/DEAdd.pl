defstar {
    name { Add }
    domain { DE }
    version { $Id$ }
    author { Bilung Lee }
    copyright {
Copyright (c) %Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { DE main library }
    desc { 
        Output the sum of those inputs with present events.
        I.e. the value of the other input is assumed to be zero.
    }
    inmulti {
        name{input}
        type{float}
    }
    output {
        name{output}
        type{float}
    }
    go {
        completionTime = arrivalTime;

        double sum = 0.0;
        InDEMPHIter inIter(input);
        InDEPort *inPort;
        while ((inPort = inIter++) != 0) {
            if (inPort->dataNew) {
                sum = sum + double(inPort->get());
            } 
        }
        
        output.put(completionTime) << sum;
    }
}

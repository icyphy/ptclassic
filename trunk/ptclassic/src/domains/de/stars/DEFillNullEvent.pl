defstar {
	name {FillNullEvent}
	domain {DE}
	version { $Id$ }
	author { Wan-Teh Chang }
	copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc { Replace absence of event to an event with zero value. }
	ccinclude { "Plasma.h" }
	inmulti {
            name{input}
            type{float}
	}
	outmulti {
            name{output}
            type{float}
	}
        begin {
            if (input.numberPorts() != output.numberPorts()) {
                Error::abortRun(*this, "The number of input PortHoles ",
                                "must match the of output PortHoles.");
                return;

            }
        }
	go {
            completionTime = arrivalTime;
            InDEMPHIter inIter(input);
            InDEPort *inPort;
            OutDEMPHIter outIter(output);
            OutDEPort *outPort;

            while ((inPort = inIter++) != 0) {
                outPort = outIter++;

                if (inPort->dataNew) {
                    outPort->put(completionTime) = inPort->get();
                } else {
                    Plasma *plasma = Plasma::getPlasma(outPort->type());
                    Particle *particle = plasma->get() ;
                    (*particle) << 0;
                    outPort->put(completionTime) = *particle;
                }
            }
        }
}

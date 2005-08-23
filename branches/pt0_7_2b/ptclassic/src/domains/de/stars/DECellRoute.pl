defstar {
	name		{ CellRoute }
	domain		{ DE }
	version		{ @(#)DECellRoute.pl	1.9	3/2/95 }
	author		{ Paul Haskell }
	location	{ DE main library }
        copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	desc {
This star reads in input NetworkCells from multiple input
sources and routes them to the appropriate output.
	}

	inmulti {	name { input }		type { message } }
	outmulti {	name { output }		type { message } }
	defstate {
		name { RoutingTable }
		type { intarray }
		default { "0 1 2 3" }
		desc { Cell destination address to output port number map. }
	}

	hinclude { "NetworkCell.h" }

	setup {
		for (int i = 0; i < RoutingTable.size(); i++) {
			if ((RoutingTable[i] < 0) ||
					(RoutingTable[i] >= output.numberPorts())) {
				Error::abortRun(*this,
						"Routing table entry out of legal range.");
				return;
		}	}
	}


	go {
		completionTime = arrivalTime;

// INPUT
		InDEMPHIter inIter(input);
		InDEPort* inPtr;
		while ((inPtr = inIter++) != 0) {
			while (inPtr->dataNew) {
				Envelope theEnvp;
				inPtr->get().getMessage(theEnvp);
				TYPE_CHECK(theEnvp, "NetworkCell");
				const NetworkCell* cellPtr = (const NetworkCell*)
						theEnvp.myData();
				const int dest = cellPtr->dest();
				if ((dest < 0) || (dest >= RoutingTable.size())) {
					Error::abortRun(*this,
							"Cell with illegal destination.");
					return;
				}
				const int port = RoutingTable[dest];

// OUTPUT
				OutDEMPHIter outIter(output);
				OutDEPort* outPtr = outIter++;
				for(int i = port; i > 0; i--) { outPtr = outIter++; }

				outPtr->put(completionTime) << theEnvp;

				inPtr->getSimulEvent();
		}	}
	} // end go{}
} //end defstar { CellRoute }

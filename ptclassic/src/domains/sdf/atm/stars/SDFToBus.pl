
defstar {
	name { ToBus }
	domain { SDF }

	descriptor {
Converts from MPH to multiple single ports or vice-versa
	}

	version { $Id$ }
	author { Allen Y. Lao }

        copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

        location  { ATM demo library }

	inmulti {
		name { input }
		type { message }
	}

	outmulti {
		name { output }
		type { message }
	}


        ccinclude { "Message.h" }

        setup {
                if (input.numberPorts() != output.numberPorts()) {
                  Error::abortRun(*this, 
                    "input and output sizes don't match.");
                  return;
                }
        }

	go {
                MPHIter next1(input);
                MPHIter next2(output);

                PortHole *p1, *p2;

                while ((p1 = next1++) != 0) {
		  p2 = next2++;
		  (*p2)%0 = (*p1)%0;
                }  // end while
	}
}



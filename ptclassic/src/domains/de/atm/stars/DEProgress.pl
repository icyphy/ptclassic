
defstar {
	name { Progress }
	domain { DE }
	desc {
Periodically print to <stdout> current time of simulation
        }

	version {$Id$}
	author { Allen Y. Lao }

	copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
	location { ATM demo library }

	explanation {
This star is used to print to <stdout> the progress of a simulation
running in the DE domain, the state \fIstep\fR indicates that for the particle
passing into the star's input at integral multiples of \fIstep\fR, a message
will be printed to indicate that system time has reached that point.
        }

	input {
		name { input }
		type { ANYTYPE }
	}

	output {
		name { output }
		type { =input }
        }
    
        defstate {
                name { update }
                type { float }
                default { "1.0" }
                desc { Step size to print system time progress }
        }

        protected {
          double timeToReach;
        }

        ccinclude { <stream.h> }

        setup {
          // The first time to reach
          timeToReach = double(update);
        }

	go {

	  completionTime = arrivalTime;
	  // pass the particle through
	  output.put(completionTime) = input.get();

          if (arrivalTime >= timeToReach)  {
            cout << "Simulation time: " << arrivalTime << "\n";
            // set the next time to reach
            timeToReach = timeToReach + double(update);
          }
        }
}


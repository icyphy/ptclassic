
ident {
#define  MAXPORTS 64
}

defstar{
       name {Throughput}
       domain {DE}
       desc { 
Measures throughput of a set of traffic-loaded trunk lines.
       }

       version {$Id$}
       author { Allen Lao }

       copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
       location {ATM demo library}

       explanation {
To be placed after an SDF compiled galaxy switch and before a
.c DENullRemove
star.  The SDF switch, for each time slot, issues an
actual packet for a line if it is active, and a "null" packet if
idle.  This star passes the packets received on its \fIinput\fR MPH
through to its \fIoutput\fR MPH, and when triggered on \fIdemand\fR
port, produces a figure corresponding to the avg. throughput over
each line as well as one for the avg. throughput over all
lines considered as a whole.  (Packets are of type
.c VoiceData
)
       }

       ccinclude { "VoiceData.h" }

       input {
              name { demand }
              type { anytype }
             }

       inmulti {
              name { input }
              type { message }
       }

       output {
              name { avgThroughput }
              type { float }
       }

       outmulti {
              name { throughput }
              type { float }
       }

       outmulti {
              name { output }
              type { message }
       }

       constructor {
	      input.triggers(output);
	      demand.triggers(avgThroughput);
	      demand.triggers(throughput);
	      input.before(demand);
       }

       protected {
	      int  realPackets[MAXPORTS];
	      int  nullPackets[MAXPORTS];
	      int  numPorts;
       }

       setup {
          if (input.numberPorts() != output.numberPorts()) {
              Error::abortRun(*this, "Inconsistent number of i/o ports");
	      return;
          }

          if (input.numberPorts() != throughput.numberPorts()) {
              Error::abortRun(*this, "Inconsistent number of i/t ports");
	      return;
          }

          numPorts = input.numberPorts();

          // initialize packet counts to zero for all lines
	  for (int i = 0; i < numPorts; i++) {
	      realPackets[i] = 0;
	      nullPackets[i] = 0;
          }
       }


       go {
	     completionTime = arrivalTime;
       
             InDEMPHIter nextp(input);
             InDEPort *iport;
             OutDEMPHIter nextq(output);
             OutDEPort *oport;
	     OutDEMPHIter nextr(throughput);
	     OutDEPort *rport;

	     Envelope  inEnv;
	     const VoiceData*  v;
      
	     int  countReal, countNull, count;
	     countReal = countNull = count = 0;

             while ((oport = nextq++) != 0) {
                iport = nextp++;

		if (iport->dataNew) {
		    iport->get().getMessage(inEnv);
		    v = (const VoiceData*) inEnv.myData();

		    // update totals as appropriate depending on whether this
		    // packet is null or not
			
		    if (v -> nulltestMessage()) {
			nullPackets[count]++;
                    }
		    else {
			realPackets[count]++;
                    }

		    oport->put(completionTime) << inEnv;
                }

		count++;
             }  // end while

	     if (demand.dataNew) {

		for (int i = 0; i < numPorts; i++) {
		    rport = nextr++;

		    countReal += realPackets[i];
		    countNull += nullPackets[i];

		    // send out Tput for each channel resp.
         	    rport -> put(completionTime) << 
			(double(realPackets[i])) /
			(double(realPackets[i] + nullPackets[i]));  
                }

		// output overall throughput
	        avgThroughput.put(completionTime) << 
		    (double (countReal)) /
		    (double (countReal + countNull)); 

             }

       } // end go()

} // end defstar { DEThroughput }



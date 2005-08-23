
ident {
#define NUMPORTS 64
}

defstar{
       name {Latency}
       domain {DE}
       version {@(#)DELatency.pl	1.7	06 Oct 1996}
       location { DE ATM library }
       author { Allen Lao }
       copyright { 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

       desc { 
Measures packet traversal-time statistics for a set of traffic-
loaded trunk lines.
       }

	htmldoc {
Produces several statistics: on a per-packet basis, it passes through
<tt>VoiceData</tt>
type packets received on <i>input</i> to <i>output</i> and also
each packet's individual traversal time on the <i>latency</i> port.
When a <i>demand</i> is detected, it produces average latency statistics
for each individual trunk line on <i>avgLatencyIndiv</i> port and over all
trunk lines on <i>avgLatencyAll</i> port.
       }

       hinclude {"VoiceData.h"}

       input {
              name { demand }
              type { anytype }
             }

       inmulti {
              name { input }
              type { message }
       }

       output {
              name { avgLatencyAll }
              type { float }
       }

       outmulti {
              name { avgLatencyIndiv }
              type { float }
       }

       outmulti {
              name { latency }
              type { float }
       }

       outmulti {
              name { output }
              type { message }
       }

       constructor {
	      input.triggers(output);
	      input.triggers(latency);
	      demand.triggers(avgLatencyAll);
	      demand.triggers(avgLatencyIndiv);
	      input.before(demand);
       }

       protected {
	      float  delay[NUMPORTS];
	      int    totalPackets[NUMPORTS];
	      int    numPorts;
       }

       code {
	      extern int voiceCheck (Envelope&, NamedObj&);
       }

       setup {
          if (input.numberPorts() != output.numberPorts()) {
              Error::abortRun(*this, "Inconsistent number of i/o ports");
	      return;
          }

          if (input.numberPorts() != latency.numberPorts()) {
              Error::abortRun(*this, "Inconsistent number of i/l ports");
	      return;
          }

          if (input.numberPorts() != avgLatencyIndiv.numberPorts()) {
              Error::abortRun(*this, "Inconsistent number of i/avg ports");
	      return;
          }

          numPorts = input.numberPorts();

          // initialize packet counts to zero for all lines
	  for (int i = 0; i < numPorts; i++) {
	      delay[i] = 0.0;
	      totalPackets[i] = 0;
          }
       }


       go {
	     completionTime = arrivalTime;
       
             InDEMPHIter nexti(input);
             InDEPort *iport;

             OutDEMPHIter nexto(output);
             OutDEPort *oport;

	     OutDEMPHIter nextlat(latency);
	     OutDEPort *latport;

	     OutDEMPHIter nextavg(avgLatencyIndiv);
	     OutDEPort *avgport;

	     Envelope  inEnv;
	     const VoiceData*  v;
      
	     float  del, delayTotal;
	     int  count, countTotal;
	     count = 0;
	     countTotal = 0;
	     delayTotal = 0.0;

             while ((oport = nexto++) != 0) {

                latport = nextlat++;
                iport = nexti++;

		if (iport->dataNew) {

		    // get input packet
	            iport->get().getMessage(inEnv);
		    if (!voiceCheck(inEnv,*this)) return;
		    v = (const VoiceData*) inEnv.myData();

		    // calculate latency for this packet
		    del = arrivalTime - (v->retStamp());

		    // update statistics
		    delay[count] = delay[count] + del;
		    totalPackets[count]++;

		    // output some stuff
		    latport->put(completionTime) << del;
		    oport->put(completionTime) << inEnv;
                }

		count++;
             }

	     if (demand.dataNew) {

		for (int i = 0; i < numPorts; i++) {

		    avgport = nextavg++;
		    countTotal += totalPackets[i];
		    delayTotal += delay[i];

		    avgport -> put(completionTime) << 
			(double(delay[i])) / (double(totalPackets[i]));

                }

		avgLatencyAll.put(completionTime) << 
			(double(delayTotal)) / (double(countTotal));

             }

       } // end go()

} // end defstar { DELatency }



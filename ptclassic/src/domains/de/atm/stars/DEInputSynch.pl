
defstar{
       name { InputSynch }
       domain { DE }
       desc { Synchronize data from input to output.  If there is
no data packet present at input ports, a "null" packet will be 
sent to the corresponding output port.  
       }

       version {$Id$}
       author { John Loh and Allen Lao }

       copyright { 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

       location {ATM demo library}

       hinclude {"VoiceData.h"}

       input {
              name { demand }
              type { anytype }
             }
       inmulti {
              name { data }
              type { message }
       }
       outmulti {
              name { output }
              type { message }
       }

       defstate {
              name { delay }
              type { float }
              default { "0.0" }
              desc { Delay from input to output }
       }
       constructor {
              delayType = TRUE;
              data.triggers();
              data.before(demand); 
       }

       protected {
	      Envelope  nEnv;
       }

       setup {
	  Envelope  dumEnv(*new VoiceData());
	  nEnv = dumEnv;

          if (data.numberPorts() != output.numberPorts())
              Error::abortRun(*this, "Inconsistent number of i/p o/p ports");
       }


       go {
             completionTime = arrivalTime + double(delay);
       
             InDEMPHIter nextp(data);
             InDEPort *iport;
             OutDEMPHIter nextq(output);
             OutDEPort *oport;
      
	     int  anyNewData;
	     anyNewData = FALSE;

             // if new packet arrives, then go ahead and iterate
             // through inputs and output nulls for  

             if (demand.dataNew) {

                 demand.dataNew = FALSE;

                 //  check to see if any new incoming data, if none no
	         //  need to enter the next while loop

		 while ((iport = nextp++) != 0) {
		     if (iport->dataNew)  {
			 anyNewData = TRUE;
                     }
                 }

                 if (anyNewData)  {
                     nextp.reset();

                     while ((oport = nextq++) != 0) {
                        iport = nextp++;
                        if (iport->dataNew) {
			  // pass through the waiting packet
                          oport->put(completionTime)=iport->get();
                        }
                        else {
			  // no new packet, pass through a null
                          oport->put(completionTime) << nEnv;
                        }
                     } // end while
                 }

             } // end "demand" dataNew if


       } // end go()
} // end defstar { DEInputSynch } 



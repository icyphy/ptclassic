defstar{
       name { NullRemove }
       domain {DE}
       desc { Remove null VoiceData packets and only output non-null packet data. }
       version {$Id$}
       author { John Loh }

       copyright { 
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

       location {ATM demo library}

       ccinclude {"VoiceData.h"}

       inmulti {
              name { data }
              type { message }
       }
       outmulti {
              name { output }
              type { message }
       }

       constructor {
              data.triggers();
       }

       code {
	      extern int voiceCheck (Envelope&, NamedObj&);
       }

       setup {
          if (data.numberPorts() != output.numberPorts())
              Error::abortRun(*this, "Inconsistent number of i/p o/p ports");
       }

       go {
             
             Envelope  inEnv;
             completionTime = arrivalTime;
       
             InDEMPHIter nextp(data);
             InDEPort *iport;
             OutDEMPHIter nextq(output);
             OutDEPort *oport;
             
             while ((oport = nextq++) != 0) {
                  iport = nextp++;
                  if (iport->dataNew)  {
                    iport->get().getMessage(inEnv);
		    if (!voiceCheck(inEnv,*this)) return;
                    VoiceData*  v = (VoiceData*) inEnv.myData();
                    if (! v->nulltestMessage())
                       oport->put(completionTime) << inEnv;
                  }
             } //while

       } //go()

} //end NullRemove

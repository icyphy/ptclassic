defstar{
       name { NullRemove }
       domain {DE}
       desc {
Removes null
.c VoiceData
messages }
       version {$Id$}
       author { John Loh }

       copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

       location {ATM demo library}

       explanation {
See comments on
.c DEInputSynch
star.  This star would be used after an
SDF-in-DE wormhole to perform inverse operation of
.c DEInputSynch
, that
is, to filter out null
.c VoiceData
messages but to allow others to pass through unchanged.
       }

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
                    const VoiceData*  v = (const VoiceData*) inEnv.myData();
                    if (! v->nulltestMessage())
                       oport->put(completionTime) << inEnv;
                  }
             } //while

       } //go()

} //end NullRemove

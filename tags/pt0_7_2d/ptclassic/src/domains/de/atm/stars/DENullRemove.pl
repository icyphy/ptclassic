defstar{
       name { NullRemove }
       domain {DE}
       desc {
Removes null
.c VoiceData
messages }
       version {@(#)DENullRemove.pl	1.6	01 Oct 1996}
       author { John Loh }

       copyright { 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}

       location { DE ATM library }

	htmldoc {
See comments on
<tt>DEInputSynch</tt>
star.  This star would be used after an
SDF-in-DE wormhole to perform inverse operation of
<tt>DEInputSynch</tt>
, that
is, to filter out null
<tt>VoiceData</tt>
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

defstar { 
	name {SrcControl}
	domain {DE}
        derivedfrom { RepeatStar }
	desc {
Communicates with an
.c MQTelephone
star to control
.c DEVoiceSrc
stars' transmissions
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
Receives 
.c VoiceData
type SendData and StopSendData packets from an 
.c MQTelephone
star to control possibly multiple 
.c DEVoiceSrc
star instances.  When idle, it accepts SendData packets containing info which
should be mapped onto
the headers of generated source packets, incoming VPI and destination
VCI numbers.  (Also, headers will have a source VCI number field.)  It will
send to the 
.c DEVoiceSrc 
stars Transmit packets to ask them to begin 
transmitting with this necessary information.  When transmission is complete
as specified by the call duration field of the original SendData packet, it
sends back to its associated
.c MQTelephone
star an Acknowledgement (ACK) packet. 
Also, while active, a StopSendData packet will terminate call activity which is
replied to with an ACK. In both cases, StopTransmit packets are sent to the
.c DEVoiceSrc
stars.
       }

       input {
               name {phoneIn}
               type {message}
       }

       output {
	       name {phoneOut}
	       type {message}
       }

       outmulti {
               name {control}
               type {message}
       }
  
       defstate {
               name {numVci}
               type {int}
               default {"1"}
               desc { VCI of associated telephone }
       }

       ccinclude { "VoiceData.h" }

       code {
               extern int voiceCheck (Envelope&, NamedObj&);
       }

       constructor {
               phoneIn.triggers();
       }

       protected {
               int    activeOrNot;     //  keep notion of state
               int    length;          //  intended duration of call
               int    feedbacksLeft;   //  unprocessed feedback events in queue for refiring
                                       //  recall this is derived from RepeatStar, the star
                                       //  uses this mechanism to keep track of its limit on
                                       //  call duration
       }

       setup {
               activeOrNot = FALSE;
               feedbacksLeft = 1;      //  recall one feedback event comes
                                       //  at time zero for RepeatStar
       }
  
       go {
               Envelope inEnv;
               OutDEMPHIter nextp(control);
               OutDEPort*   oport;

               if (activeOrNot == FALSE) {     //  sources idle

                   if (!phoneIn.dataNew) {
                       feedbacksLeft--;
                       if (feedbacksLeft < 0)  {
                           Error :: abortRun (*this,
                                    "how can no. of feedback events be negative?");
                           return;
                       }
                       return;
                   }

                   phoneIn.get().getMessage(inEnv);
                   if (!voiceCheck (inEnv, *this)) return;
                   const VoiceData*  v = (const VoiceData*) inEnv.myData();
                
                   if (!v->isitSDMessage())  {
                       Error :: abortRun (*this,
                                "expected a sendDataPacket in idle state.");
                       return;
                   }

                   int  vpi, vciSrc, vciDst;
                   unsigned char* ptr = v->asVoice();

                   // read info from SDP to put into Transmit packet
                   // record specified call length
                   length = ptr[6];
                   vciDst = ptr[7];
                   vciSrc = int(numVci);
                   
                   if (vciSrc%3 == 1 || vciSrc%3 == 2)
                       vpi = vciSrc%3;
                   else
                       vpi = 3;

                   LOG_NEW; Envelope  oEnv(*new VoiceData ("TRP", &vciSrc, &vciDst, &vpi));
                   completionTime = arrivalTime;
                   
                   // send the transmit packet to all connected sources
                   while ((oport = nextp++) != 0)
                       (*oport).put(completionTime) << oEnv;

                   refireAtTime (completionTime + double(length));
		   //  keep track of the refire feedback particles
                   feedbacksLeft++;
                   activeOrNot = TRUE;

               }
               else  {
 
                   if (phoneIn.dataNew) {

                       //  the caller interrupted by MQ telephone
                       phoneIn.get().getMessage(inEnv);
                       if (!voiceCheck (inEnv, *this)) return;
                       const VoiceData*  v1 = (const VoiceData*) inEnv.myData();
                    
                       if (!v1->isitSSDMessage()) {
                           Error :: abortRun (*this,
                                    "expected a stopSendData packet!");
                           return;
                       }

                       completionTime = arrivalTime;

                       // acknowledge the interruption
                       LOG_NEW; Envelope  oEnv1 (*new VoiceData("ACK"));
                       phoneOut.put(completionTime) << oEnv1;

                       LOG_NEW; Envelope  oEnv2 (*new VoiceData("STRP"));
                       // send the stop transmit packet to all sources
                       while ((oport = nextp++) != 0)
                           (*oport).put(completionTime) << oEnv2;

                       activeOrNot = FALSE;

                   }
                   else {

		       // time-out, time for this phone to end the conversation

                       feedbacksLeft--;
                       if (feedbacksLeft < 0)  {
                           // impossible to have negative number
                           Error :: abortRun (*this,
                                    "how can no. of feedback events be negative?");
                           return;
                       }

                       feedbackIn->get();

                       if (feedbacksLeft != 0)  {
                           // this situation indicates that there are leftover events
                           // in the feedback arc, would happen if Caller A talks to
                           // Caller B, B terminates (A's feedback event is still in
                           // queue for ending the call), and another call occurs between
                           // A and C before this feedback event is processed, then 
                           // Caller A hangs up on C, i.e. the feedback event from the
                           // previous call has been processed instead of the current call!
                           // so we take suitable precautions
                           // Error :: error (*this,
                           //       "leftover feedback refirings in transmission!");
                           return;
                       }
                       
                       completionTime = arrivalTime;

                       // send ACK to phone to let it know of desire to "hang up"
                       LOG_NEW; Envelope oEnv3 (*new VoiceData ("ACK"));
                       phoneOut.put(completionTime) << oEnv3;

                       LOG_NEW; Envelope oEnv4 (*new VoiceData("STRP"));
                       // force sources to cease transmissions
                       while ((oport = nextp++) != 0)
                           (*oport).put(completionTime) << oEnv4;

                       activeOrNot = FALSE;
                   }
               }

    }   // end go
}   // end defstar


defstar {
	name {VoiceSrc}
	domain {DE}
	derivedfrom { RepeatStar }
	desc {
Generates a voice process for a single caller and recognizes
.c VoiceData
prompts to begin and cease packet transmission.
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
This star produces dummy voice packet outputs modeling a voice process
where silence periods are exponentially distributed in length
with mean \fIsilenceExpMean\fR.  The talkspurts are geometrically
distributed with the mean number of packets per talkspurt set by the
state \fImeanPacketsSpurt\fR.  Then there is a state for the packetization
period called \fIpackPeriod\fR. Naturally, one can figure the mean duration
of a talkspurt from the \fImeanPacketsSpurt\fR and \fIpackPeriod\fR states.
Also, the packet is time-stamped and identified by connection.

It recognizes
.c VoiceData 
type input prompts: Transmit Packets
and StopTransmitPackets. Transmit packets send as info. incoming VPI,
source VCI number, and destination VCI number.  Preceding a transmission
is a Start-of-Transmission packet (SOT), and after a transmission has
been completed, an End-of-Transmission (EOT) packet is sent.
	}

        input {
                name {input}
                type {message}
        }

	output {
		name {output}
		type {message}
	}

	defstate {
		name {packPeriod}
		type {float}
		default {".016"}
		desc { Voice packetization period }
	}

        defstate {
                name {silenceExpMean}
                type {float}
                default {".650"}
                desc {Exp. mean for a silence period in seconds }
        }

        defstate {
                name {meanPacketsSpurt}
                type {int}
                default {"22"}
                desc {Mean no. of packets per talkspurt}
        }


        hinclude { <NegExp.h>, <Uniform.h> }

	ccinclude { "VoiceData.h", <ACG.h> }

	code {
                extern ACG* gen;
                extern int voiceCheck (Envelope&, NamedObj&);
	}

        constructor {
                randomExp = NULL;
                randomUnif = NULL;
        }

	destructor {
		if (randomExp) { LOG_DEL; delete randomExp; }
		if (randomUnif) { LOG_DEL; delete randomUnif; }
        }
	   
	protected {
                NegativeExpntl *randomExp;
                Uniform        *randomUnif;

                int            talkingOrNot; 
                int            vciSrc, vciDst, vpi;
	}

	setup {
                if (randomExp) { LOG_DEL; delete randomExp; }
                LOG_NEW; randomExp = new NegativeExpntl(double(silenceExpMean),gen);

                if (randomUnif) { LOG_DEL; delete randomUnif; }
                LOG_NEW; randomUnif = new Uniform(double(0),double(1),gen);

                talkingOrNot = FALSE;
	}

	go {
		Envelope inEnv;

                if (talkingOrNot == FALSE)  {       //  idle state
                  
                    if (!input.dataNew)  {
                        //  leftover feedback event is possible
                        feedbackIn->get();
                        return;
                    }

                    //  expect a TRANSMIT packet to begin voice transmission
                    input.get().getMessage(inEnv);
                    if (!voiceCheck (inEnv, *this)) return;
                    const VoiceData*  v = (const VoiceData*) inEnv.myData();

                    if (!v->isitTRMessage()) {
                        Error :: abortRun (*this,
                                 "expected a Transmit packet in idling state.");
                        return;
                    }

                    unsigned char* ptr = v->asVoice();
                    //  record info to be mapped into voice packet headers
                    vciSrc = ptr[6];
                    vciDst = ptr[7];
                    vpi    = ptr[8];

                    // mark the start of the transmission with an "SOT" packet
                    completionTime = arrivalTime;
                    LOG_NEW; VoiceData* w = new VoiceData ("SOT", &vciSrc, &vciDst);
		    w -> setStamp(completionTime);
                    Envelope oEnv(*w);
                    output.put(completionTime) << oEnv;

                    // prepare to refire to begin data transmission
                    completionTime += double(packPeriod);
                    refireAtTime(completionTime);
                    talkingOrNot = TRUE;

                }
                else  {       //  talking state
  
                    if (input.dataNew) {

                        //  transmission interrupted
                        input.get().getMessage(inEnv);
                        if (!voiceCheck (inEnv, *this)) return;
                        const VoiceData*  v1 = (const VoiceData*) inEnv.myData();

                        if (!v1->isitSTRMessage()) {
                            Error :: abortRun (*this,
                                     "expected a stopTransmit packet when talking.");
                            return;
                        }

                        // send out an EOT packet to mark end of transmission
                        completionTime = arrivalTime + double(packPeriod);
                        LOG_NEW; VoiceData*  w1 = new VoiceData("EOT", &vciSrc, &vciDst);
			w1 -> setStamp(completionTime);
                        Envelope oEnv1(*w1);
                        output.put(completionTime) << oEnv1;
                        talkingOrNot  = FALSE;
            
                    }
                    else {
                  
			//  received a token on the "feedback"port caused by a
			//  call to refire this star, transmit a packet with appropiate
			//  header information
                        LOG_NEW; VoiceData*  w2 = new VoiceData();
                        unsigned char*  ptz = w2->asVoice();
                        ptz[0] = vpi;  ptz[2] = vciSrc;  ptz[3] = vciDst;

			completionTime = arrivalTime;
			w2 -> setStamp(completionTime);
                        Envelope  oEnv2(*w2);
                        output.put(completionTime) << oEnv2;
                        double d1, d2;
                        d2 = (*randomUnif)();
    
                        if (d2 < 1.0/double(int(meanPacketsSpurt)))  {
                            d1 = (*randomExp)();
                            completionTime += d1;
                        }
                        else {
                            completionTime += double(packPeriod);
                        }

                        refireAtTime(completionTime);

                    }  // packet transmission vs. interruption "if" check
                }  // testing for idle or talking state
            

    }  // go method
}  // defstar

                           

defstar {
	name {VoiceFl}
	domain {DE}
	desc {
Reads in voice packet and sends out its destination VCI number
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
Star is to be placed at switch output and feeds into "graphing" star,
to plot destination VCI field of incoming
.c VoiceData 
packets
        }

        input {
                name {input}
                type {message}
        }

	output {
		name {output}
		type {float}
	}
  
        defstate {
                name {field}
                type {int}
                default {"2"}
                desc {
Field of
.c VoiceData
packets to output }
        }

	ccinclude { "VoiceData.h" }

        code {
                extern int voiceCheck (Envelope&, NamedObj&);
        }

	go {
                Envelope  inEnv;
                input.get().getMessage(inEnv);
                if (!voiceCheck (inEnv, *this)) return;
                const VoiceData*  v = (const VoiceData*) inEnv.myData();
                completionTime = arrivalTime;

		// Send out data if not start-of-transmission nor end-of-
		// transmission message

		if (!v->isitSOTMessage() && !v->isitEOTMessage()) {
                    unsigned char* ptr = v->asVoice();
                    int  i0 = ptr[int(field)];
                    output.put(completionTime) << double(i0);
                }

        }   
}


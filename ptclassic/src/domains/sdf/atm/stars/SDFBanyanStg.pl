defstar {
    name      { BanyanStg }
    domain    { SDF }
    desc      { 2 x 2 Banyan switching element with internal queueing }
    version   {$Id$}
    author    { John Loh }

    copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
    location  { ATM demo library }

    explanation {  
Accept two input packets, and route them to the proper
output based on the information in the headers.
The actual bit of the VPI being used for routing is based
on the state \fIstage\fR set in 
.c SDFBanyanGal
compiled galaxy.  This star maintains
internal FIFO queues for each of the two input terminals.
Note the special processing required due to null packets.
    }

    hinclude { "NewQueue.h", "VoiceData.h", <Uniform.h> }

    ccinclude { <ACG.h> }

    input {
        name { input1 }
        type { message }
    }
    input {
        name { input2 }
        type { message }
    }
    output {
        name { output1 }
        type { message }
    }
    output {
        name { output2 }
        type { message }
    }

    defstate {
        name    { stage }
        type    { int }
        default { 1 }
        desc    { Stage within Banyan (1,2,3). }
    }
    defstate {
        name    { capacity }
        type    { int }
        default { 50 }
        desc    { Max number of particles in queue }
    }

    private {
      NewQueue queue1;
      NewQueue queue2;
    }

    protected {
      Uniform*  random;
      Envelope  nEnv;
    }

    code {
      extern ACG* gen;
      extern int voiceCheck (Envelope&, NamedObj&);
    }

    constructor {
      random = 0;
    }
    
    destructor {
      LOG_DEL; delete random;
    }

    setup {
      LOG_DEL; delete random;
      LOG_NEW; random = new Uniform(0,1,gen);

      Envelope dumEnv(*new VoiceData());
      nEnv = dumEnv;

      queue1.initialize(int(capacity));
      queue2.initialize(int(capacity));
    }

    go {
        Envelope inEnv1, inEnv2;
        MessageParticle  alpha, beta, copy1, copy2, temp1, temp2;
       
        alpha.initialize(); beta.initialize();
        copy1.initialize(); copy2.initialize();
        temp1.initialize(); temp2.initialize();

        copy1 = input1%0;
        (copy1).getMessage(inEnv1);

        if (!voiceCheck (inEnv1,*this)) return;

        if (!(((const VoiceData*)(inEnv1.myData())) -> nulltestMessage())) {
	   // here inEnv1 is non-null so queue it and get what's at queue's head
           queue1.putq(input1%0);
           queue1.getq(alpha);
           (alpha).getMessage(inEnv1);
        }
        else {
	   // if inEnv1 null, check queue. 
           if (!queue1.empty()) {
              queue1.getq(alpha);
              (alpha).getMessage(inEnv1);
           }
        }

        copy2 = input2%0;
        (copy2).getMessage(inEnv2);

	if (!voiceCheck (inEnv2,*this)) return;

        if (!(((const VoiceData*)(inEnv2.myData())) -> nulltestMessage())) {
	   // if inEnv2 non-null queue it and get what's at queue's head
	   queue2.putq(input2%0);
           queue2.getq(beta);
           (beta).getMessage(inEnv2);
        }
        else {
	   // if inEnv2 null, check queue
           if (!queue2.empty()) {
	      queue2.getq(beta);
	      (beta).getMessage(inEnv2);
           }
        }

        const VoiceData* inVoice1 = (const VoiceData*) inEnv1.myData();
        const VoiceData* inVoice2 = (const VoiceData*) inEnv2.myData();
        unsigned char * ptr1 = inVoice1->asVoice(); 
        unsigned char * ptr2 = inVoice2->asVoice(); 

        int n1,n2, r1down, r2down;

        n1 = 0;
        n2 = 0;

        r1down = 0; //route input 1 down if 1 
        r2down = 0; //route input 2 down if 1

        if (inVoice1->nulltestMessage()) n1 = 1; 
        if (inVoice2->nulltestMessage()) n2 = 1; 

        if ((int(ptr1[0])>>(int(stage)-1))&01) r1down = 1;
        if ((int(ptr2[0])>>(int(stage)-1))&01) r2down = 1;

        if (n1 && n2) {  
          //both data from queue are null, so send nulls out
          (output1%0) << inEnv1; 
          (output2%0) << inEnv2; 
        } 

        else if (n1) {  
          // first is null, so send out second and place null on other output
          if (r2down) {
             (output1%0) << inEnv1; // null
             (output2%0) << inEnv2; // route msg2 down
          }
          else {
             (output1%0) << inEnv2; // route msg2 up 
             (output2%0) << inEnv1; // null
          }  
        }

        else if (n2) {
          // second is null, so send out first and place null on other output
          if (r1down) {
             (output1%0) << inEnv2; //null
             (output2%0) << inEnv1; //route msg1 down
          }
          else {
             (output1%0) << inEnv1; //route msg1 up 
             (output2%0) << inEnv2; //null
          }  
        }

        else if (r1down != r2down) {
	  //  both msgs are routed to different outputs
          if (r1down) {
             (output1%0) << inEnv2; //route msg2 up 
             (output2%0) << inEnv1; //route msg1 down
          }
          else { 
             (output1%0) << inEnv1; //route msg1 up 
             (output2%0) << inEnv2; //route msg2 down
          }
        }

        else { 

          // both msgs want to go to the same output (decide with coin toss)

          double p1 = (*random)();

          if (r1down) {
             (output1%0) << nEnv;   // send null on upper output 
             if (p1 < 0.5) {
               (output2%0) << inEnv1;  // send first msg on lower output
               temp1 << inEnv2;
               queue2.puthead(temp1);
             }
             else {
               (output2%0) << inEnv2;  // send second msg on lower output
               temp1 << inEnv1;
               queue1.puthead(temp1);
             }
          }

          else {
             (output2%0) << nEnv;  // send null on lower output
             if (p1 < 0.5) {
               (output1%0) << inEnv1;  // send first msg on upper output
               temp2 << inEnv2;
               queue2.puthead(temp2);
             }
             else {
               (output1%0) << inEnv2;  // send second msg on upper output
               temp2 << inEnv1;
               queue1.puthead(temp2);
             }
           }

         }

    } // end go{}

} // end defstar

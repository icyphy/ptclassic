defstar {
    name      { Controller }
    domain    { SDF }
    desc      { Switch routing table }

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
This star maintains a routing table which is updated by the
.c MQControl
star.  For input packets, they have their headers reassigned according
to the table's data.
    }

    hinclude { "VoiceData.h", <ACG.h> }

    inmulti {
        name { indata }
        type { message }
    }

    input {
        name { tableupdate }
        type { message }
    }

    outmulti {
        name { outdata }
        type { message }
    }

    output {
        name { ackmq }
        type { message }
    }

    defstate {
        name    { numcpes }
        type    { int }
        default { 12 }
	desc    { Number of CPE's in simulation }
    }

    protected {
      Envelope  nEnv;
      int **    srttable;
    }

    code {
      extern int voiceCheck (Envelope&, NamedObj&);
      extern ACG* gen;
    }

    constructor {
      srttable = 0;
    }

    setup {
      Envelope dumEnv(*new VoiceData());
      nEnv = dumEnv;

      if ( srttable ) {
         for (int i = 0; i < int(numcpes); i++) {
	   LOG_DEL; delete [] srttable[i];
         }
         LOG_DEL; delete [] srttable;
      }

      LOG_NEW; srttable = new int* [int(numcpes)];
      for (int i = 0; i < int(numcpes); i++) {
	 LOG_NEW; srttable[i] = new int [indata.numberPorts()];
      }
      
      for (i = 0; i < int(numcpes); i++)
	 for (int j = 0; j < indata.numberPorts(); j++)
	     srttable[i][j] = 0;
    }

    destructor {
      for (int i = 0; i < int(numcpes); i++) {
	  LOG_DEL; delete [] srttable[i];
      }
      LOG_DEL; delete [] srttable;
    }

    go {
        Envelope inEnv, updateEnv;

        int vci,ivci,ovci,scpe,dcpe;

        vci = 0;
        ivci = 0;
        ovci = 0;
        scpe = 0;
        dcpe = 0;
       
        (tableupdate%0).getMessage(updateEnv);
	if (!voiceCheck (updateEnv,*this)) return;
        const VoiceData * inVoice1 = (const VoiceData*) updateEnv.myData();
        unsigned char *ptr1 = inVoice1->asVoice();

        if (inVoice1->nulltestMessage()) { 
           ackmq%0 << nEnv;
        }
        else {
           // Do the update
           // Two updates must be performed
           ivci = (int) ptr1[0]; //obtain IVCI/VPI value
           ovci = (int) ptr1[1]; //obtain OVCI/VPI value
           scpe = (int) ptr1[2]; //obtain SCPE#
           dcpe = (int) ptr1[3]; //obtain DCPE#
  
           // make table updates
           srttable[scpe-1][ivci-1] = ovci; //process for outgoing pkts
           srttable[dcpe-1][ovci-1] = ivci; //process for incoming pkts

           // Now must send an ack packet to the MQcontroller
           LOG_NEW; VoiceData *op = new VoiceData("ACK");
           Envelope  oEnv(*op);
           ackmq%0 << oEnv;
        }

        MPHIter nexti(indata);
        MPHIter nexto(outdata);
        PortHole* p;

        while((p = nexti++) != 0) {

           //Read in data packet
           ((*p)%0).getMessage(inEnv);
           const VoiceData * inVoice2 = (const VoiceData*) inEnv.myData();

           // Check to see if input packet is null

           if (inVoice2->nulltestMessage()) { 
              // Assign null packet
              (*nexto++)%0 << nEnv;
              vci++;

           }
           else {

              VoiceData * inVoice3 = (VoiceData*) inEnv.writableCopy();
              unsigned char *ptr3 = inVoice3->asVoice();

              // Packet not null, so do reassignment
              // Locate value in srttable
              scpe = int(ptr3[2]);

              // Assign new value
              ptr3[0] = (unsigned char) (srttable[scpe-1][vci] - 1);

	      // Send out the 
              Envelope  dEnv(*inVoice3);
              (*nexto++)%0 << dEnv;
              vci++;

           }  // end if

        }  // end while

    }  // end go

}  // end defstar


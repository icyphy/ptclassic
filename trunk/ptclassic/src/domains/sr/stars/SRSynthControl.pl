defstar {
  name { SynthControl }
  domain { SR }
  desc {
A polyphonic synthesizer control
  }
  version { $Id$ }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }

  htmldoc {
  }

  input {
    name { onPitch }
    type { int }
    desc { Pitch of a Note On command (0-127, 60 is middle C) }
  }
  input {
    name { onVelocity }
    type { int }
    desc { Velocity of a Note On command (0-127) }
  }

  input {
    name { offPitch }
    type { int }
    desc { Pitch of a Note Off command (0-127, 60 is middle C) }
  }

  input {
    name { pitchBend }
    type { int }
    desc { Pitch bend for the channel.  8192 is neutral. }
  }

  input {
    name { reset }
    type { int }
    desc { Present to reset the system }
  }

  outmulti {
    name { frequency }
    type { float }
    desc { Frequency values for each voice }
  }

  outmulti {
    name { velocity }
    type { float }
    desc { Velocity values for each voice }
  }

  inmulti {
    name { done }
    type { int }
    desc { Done flag for each voice }
  }

  defstate {
    name { AFreq }
    type { float }
    default { "440.0" }
    desc { Frequency output for the A below Middle C }    
  }

  defstate {
    name { velocityScale }
    type { float }
    default { "0.125" }
    desc { Maximum pressure velocity output }
  }

  defstate {
    name { debug }
    type { string }
    default { "NO" }
    desc { Print real-time event messages }
  }

  ccinclude { <math.h>, <stream.h> }

  protected {
    // Number of ports on frequency, velocity, and done multiports
    int numVoices;

    // Array of pointers to the done input ports
    InSRPort ** donePort;

    // Array of pointers to the frequency output ports
    OutSRPort ** frequencyPort;

    // Array of pointers to the velocity output ports
    OutSRPort ** velocityPort;

    // Array of pitches, one per active voice.
    //
    // @Description These are MIDI pitch values (1-127) for voices
    // that are currently sounding, and zero for idle channels
    int * voicePitch;    

    // Array of velocities, one per active voice
    //
    // @Description These are MIDI velocities, zero after a note off.
    int * voiceVelocity;

    // Array translating pitches to frequencies
    double frequencyOfPitch[128];

    // Array translating MIDI velocities to floating velocities
    double velocityOfVelocity[128];

    // Current pitch translation value.  Value multiplies all frequencies.
    double frequencyScale;

    // Flag indicating whether debugging messages should be printed
    int printDebugging;
  }

  constructor {
    donePort = NULL;
    frequencyPort = NULL;
    velocityPort = NULL;
    voicePitch = NULL;
    voiceVelocity = NULL;
    numVoices = 0;
    printDebugging = 0;

  }

  begin {
    resetVoices();
    if ( strcmp((const char *) debug, "YES" ) == 0 ) {
      printDebugging = 1;
    } else {
      printDebugging = 0;
    }
  }

  setup {
    delete [] donePort;
    delete [] frequencyPort;
    delete [] velocityPort;

    delete [] voicePitch;
    delete [] voiceVelocity;

    // Fill the pitch array with equal-tempered pitch values

    for ( int pi = 128 ; --pi >= 0 ; ) {
	frequencyOfPitch[pi] = double(AFreq) * exp((pi-56.0)*log(2.0)/12.0);
	/*	cout << "Pitch " << pi << " frequency "
	     << frequencyOfPitch[pi] << '\n'; */
    }

    // Fill the velocity array with scaled velocity values

    for ( int v = 128 ; --v >= 0 ; ) {
      velocityOfVelocity[v] = double(v) / 127.0 * double(velocityScale);
    }

    // Verify the number of ports

    if ( done.numberPorts() != frequency.numberPorts() ||
	 frequency.numberPorts() != velocity.numberPorts() ||
	 done.numberPorts() == 0 ) {
      Error::abortRun( *this, "Number of multi ports inconsistent or zero");
      return;
    }

    // Create arrays of pointers to the ports
    
    numVoices = done.numberPorts();

    // cout << "Found " << numVoices << " voices on an SRSynthControl star\n";

    donePort = new InSRPort* [numVoices];
    frequencyPort = new OutSRPort* [numVoices];
    velocityPort = new OutSRPort* [numVoices];      

    int i;
    PortHole * p;

    MPHIter nextdone(done);
    i = 0;
    while ( (p = nextdone++) != 0 ) {
      donePort[i++] = (InSRPort *)p;
      // Set this input port to be independent
      ((InSRPort *)p)->independent();
    }

    MPHIter nextfrequency(frequency);
    i = 0;
    while ( (p = nextfrequency++) != 0 ) {
      frequencyPort[i++] = (OutSRPort *)p;
    }

    MPHIter nextvelocity(velocity);
    i = 0;
    while ( (p = nextvelocity++) != 0 ) {
      velocityPort[i++] = (OutSRPort *)p;
    }

    voicePitch = new int[numVoices];
    voiceVelocity = new int[numVoices];     

  }

  public {
    void resetVoices();
  }

  code {

    void SRSynthControl::resetVoices() {
      if ( voicePitch != NULL ) {
	for ( int i = numVoices ; --i >= 0 ; ) {
	  voicePitch[i] = voiceVelocity[i] = 0;
	}
      }
      frequencyScale = 1.0;
    }

  }

  destructor {
    delete [] donePort;
    delete [] frequencyPort;
    delete [] velocityPort;

    delete [] voicePitch;
    delete [] voiceVelocity;
  }

  go {
    int i;

    // cout << "Go called on SynthControl\n";

    if ( onPitch.known() && onVelocity.known() &&
	 offPitch.known() && reset.known() ) {
	// !(frequencyPort[0]->known()) ) {

      // cout << "Entered if in SynthControl\n";

      if ( reset.present() ) {
	resetVoices();
      }
     
      if ( onPitch.present() && onVelocity.present() ) {

	// Have a note on command -- find an empty voice and play it

	for ( i = numVoices ; --i >= 0 ; ) {
	  if ( voicePitch[i] == 0 ) {
	    voicePitch[i] = int(onPitch.get());
	    voiceVelocity[i] = int(onVelocity.get());
	    if ( printDebugging ) {
	      cout << "Note on: pitch " << voicePitch[i] << " velocity "
		   << voiceVelocity[i] << " voice " << i << '\n';
	    }
	    break;
	  }
	}

      }

      if ( offPitch.present() ) {

	// Have a note off command -- find the pitch and set its velocity
	// to zero

	int p = int(offPitch.get());
	for ( i = numVoices ; --i >= 0 ; ) {

	  if ( voicePitch[i] == p ) {
	    voiceVelocity[i] = 0;
	    if ( printDebugging ) {
	      cout << "Note off: pitch " << voicePitch[i] << " voice "
		   << i << '\n';
	    }
	  }
	}
      }

      if ( pitchBend.present() ) {
	int pb = int(pitchBend.get());
	frequencyScale = exp( (pb - 8192) * log(2.0)/(8192 * 6) );
	if ( printDebugging ) {
	  cout << "Pitch Bend: " << pb << " (" << frequencyScale << ")\n";
	}
      }

      // Emit the frequencies and velocities for the voices

      for ( i = numVoices ; --i >= 0 ; ) {
	frequencyPort[i]->emit() << frequencyOfPitch[voicePitch[i]] * frequencyScale;
	velocityPort[i]->emit() << double(voiceVelocity[i]) / 256.0;
      }

    }

  }

  tick {
    // cout << "Calling tick with " << numVoices << " voices\n";

    // Check the "done" status of each voice, clearing the pitch of
    // any that are non-zero

    for ( int i = numVoices ; --i >= 0 ; ) {
      if ( donePort[i]->present() && int(donePort[i]->get()) != 0 &&
	  voicePitch[i] != 0 ) {
	voicePitch[i] = voiceVelocity[i] = 0;
	if ( printDebugging ) {
	  cout << "Done: voice " << i << '\n';
	}
      }
    }

    /* 
    for ( i = 0 ; i < numVoices ; i++ ) {
      cout << voicePitch[i] << ' ' << voiceVelocity[i] << ' ';
    }
    cout << '\n';
    */

  }

}

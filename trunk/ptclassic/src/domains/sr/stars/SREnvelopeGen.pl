defstar {
  name { EnvelopeGen }
  domain { SR }
  desc {
An envelope generator for FM sound synthesis
  }
  version { $Id$ }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }

  input {
    name { velocity }
    type { int }
    desc { Velocity of the struck note (0-127), or absent if not being played }
  }

  input {
    name { controller }
    type { int }
    desc { Controller number }
  }

  input {
    name { controlValue }
    type { int }
    desc { New value for the controller }
  }

  output {
    name { done }
    type { int }
    desc { True when the voice has stopped sounding }
  }

  output {
    name { amplitude }
    type { float }
    desc { Amplitude of generated tone }
  }

  output {
    name { index }
    type { float }
    desc { Index of modulation (amplitude of modulating signal) for the tone }
  }

  state {
    name { controllers }
    type { IntArray }
    desc { Controller numbers for attack, decay, sustain, release, and index }
    default { "73 74 70 72 71" }    
  }

  state {
    name { envelopeState }
    type { int }
    desc { State (quiet, attack, decay, or release) of the generated tone }
    default { "0" }
  }

  state {
    name { currentAmplitude }
    type { float }
    desc { Current amplitude of the envelope }
    default { "0" }
  }

  state {
    name { amplitudeScale }
    type { float }
    desc { Scale factor for amplitude (controlled by note velocity) }
    default { "1.0" }
  }

  state {
    name { attackRate }
    type { float }
    desc { Rate of attack (constant added in each cycle, scale added in) }
    default { "0.5" }
  }

  state {
    name { decayRate }
    type { float }
    desc { Rate of decay (scale factor for waveform per cycle) }
    default { "0.9" }
  }

  state {
    name { sustainLevel }
    type { float }
    desc { Aympototic level of sustain (0.0 - 1.0) }
    default { "0.2" }
  }

  state {
    name { releaseRate }
    type { float }
    desc { Rate of release (constant subtracted in each cycle) }
    default { "0.04" }
  }

  state {
    name { indexScale }
    type { float }
    desc { Scaling factor for modulation index versus envelope }
    default { "9.0" }
  }

  defstate {
    name { debug }
    type { string }
    default { "NO" }
    desc { Print real-time event messages }
  }


  public {

    // Envelope states

    enum envelopeStates {
      quietState = 0,
      attackState = 1,
      decayState = 2,
      releaseState = 3
    };

    // Index into the controller number state (array) of each controller

    enum controllerNumbers {
      attackController = 0,
      decayController = 1,
      sustainController = 2,
      releaseController = 3,
      indexController = 4
    };

  }

  ccinclude { <stream.h> }

  protected {
    // Flag indicating whether debugging messages should be printed
    int printDebugging;
  }

  constructor {
    printDebugging = 0;
  }

  begin {
    if ( strcmp((const char *) debug, "YES" ) == 0 ) {
      printDebugging = 1;
    } else {
      printDebugging = 0;
    }
  }

  go {
    if ( !amplitude.known() ) {
      // Do this only if we haven't figured out the amplitude

      if ( controller.present() ) {

	// Handle the controller change -- modify the appropriate values
	
	int ctrl = int(controller.get());
	int val = int(controlValue.get());
	
	if ( ctrl == controllers[attackController] ) {
	  attackRate = 10.0 / (val+1);
	  if ( printDebugging ) {
	    cout << "Changed attackRate to " << attackRate << '\n';
	  }
	}

	if ( ctrl == controllers[decayController] ) {
	  decayRate = val / 127.0;
	  if ( printDebugging ) {
	    cout << "Changed decayRate to " << decayRate << '\n';
	  }
	}

	if ( ctrl == controllers[sustainController] ) {
	  if ( printDebugging ) {
	    cout << "Changed sustainLevel to " << sustainLevel << '\n';
	  }
	  sustainLevel = val / 127.0;
	}
	
	if ( ctrl == controllers[releaseController] ) {
	  if ( printDebugging ) {
	    cout << "Changed releaseRate to " << releaseRate << '\n';
	  }
	  releaseRate = 1.0 / (val+1);
	}

	if ( ctrl == controllers[indexController] ) {
	  if ( printDebugging ) {
	    cout << "Changed indexScale to " << indexScale << '\n';
	  }
	  indexScale = val / 8.0;
	}

      }

      if ( !velocity.present() ) {
	if ( int(envelopeState) == attackState ||
	     int(envelopeState) == decayState ) {

	  // Note has been released -- make sure we're releasing
	  // if the note in the attack or decay modes

	  envelopeState = int(releaseState);
	}
      } else {
	// Velocity is present -- make sure to start the note
	if ( int(envelopeState) == quietState ||
	     int(envelopeState) == releaseState ) {
	  envelopeState = int(attackState);
	  amplitudeScale = int(velocity.get()) / 127.0;
	}
      }

      switch ( int(envelopeState) ) {

      case quietState:
	break;

      case attackState:
	currentAmplitude = double(currentAmplitude) + double(attackRate);
	if ( double(currentAmplitude) >= 1.0 ) {
	  envelopeState = int(decayState);
	}
	break;

      case decayState:
	currentAmplitude = (double(currentAmplitude) - double(sustainLevel))
	  * double(decayRate) + double(sustainLevel);
	break;

      case releaseState:
	currentAmplitude = double(currentAmplitude) - double(releaseRate);
	if ( double(currentAmplitude) <= 0.0 ) {
	  currentAmplitude = double(0.0);
	  envelopeState = int(quietState);
	}
	break;

      default:
	break;
      }

      if ( int(envelopeState) == quietState ) {
	done.emit() << 1;
      } else {
	done.makeAbsent();
      }
      amplitude.emit() << double(currentAmplitude) * double(amplitudeScale);
      index.emit() << double(currentAmplitude) * double(amplitudeScale)
	* double(indexScale);
      
    }

  }

}



defstar {
    name { MIDIin }
    domain { SR }
    desc {
An interpreter for the MIDI protocol.  It takes an incoming MIDI stream
and fans it out to Note On and Note Off commands.
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
	name { in }
	type { int }
	desc { A stream of MIDI bytes }
    }

    output {
	name { onPitch }
	type { int }
	desc { Pitch of a Note On command (0-127, 60 is middle C) }
    }
    output {
	name { onVelocity }
	type { int }
	desc { Velocity of a Note On command (0-127) }
    }
    output {
	name { onChannel }
	type { int }
	desc { Channel of a Note On command (0-15) }
    }

    output {
	name { offPitch }
	type { int }
	desc { Pitch of a Note Off command (0-127, 60 is middle C) }
    }
    output {
	name { offVelocity }
	type { int }
	desc { Velocity of a Note Off command (0-127) }
    }
    output {
	name { offChannel }
	type { int }
	desc { Channel of a Note Off command (0-15) }
    }

    output {
	name { pitchBend }
	type { int }
	desc { Pitch bend value, 8192 is neutral }
    }
    output {
	name { pitchChannel }
	type { int }
	desc { Channel of the Pitch Bend command (0-15) }
    }

    output {
	name { reset }
	type { int }
	desc { Present when the system reset (FF) command arrives }
    }

    state {
	name { lastStatus }
	type { int }
	default { "255" }
	desc { The most-recently-transmitted non-realtime status byte }
    }

    state {
	name { byteNum }
	type { int }
	default { "0" }
	desc { The number of the next data byte in the sequence. }
    }

    state {
      name { lastPitch }
      type { int }
      default { "60" }
      desc { The most recently observed pitch value }
    }

    private {
	int nextStatus;
	int nextByteNum;
    }

    public {
      inline int isRealtime( int i ) const {
	return i >= 0xf8 && i <= 0xff;
      }

      inline int isStatus( int i ) const {
	return i & 0x80;
      }

      inline int isSystem( int i ) const {
	return i >= 0xf0 && i <= 0xff;
      }

      // MIDI status bytes.  Not all of these are interpreted

      enum statusBytes {

	// Voices

	NoteOff = 0x80,
	NoteOn = 0x90,
	NoteAftertouch = 0xa0,
	SetParameter = 0xb0,
	ProgramChange = 0xc0,
	ChannelAftertouch = 0xd0,
	ChannelPitchwheel = 0xe0,

	// System Common

	SystemExclusive = 0xf0,
	MTCQuarterFrame = 0xf1,
	SongPosition = 0xf2,
	SongSelect = 0xf3,
	TuneRequest = 0xf6,
	EndOfSystemExclusive = 0xf7,

	// System Realtime
	TimingClock = 0xf8,
	Start = 0xf9,
	Continue = 0xfa,
	Stop = 0xfb,
	ActiveSensing = 0xfe,
	SystemReset = 0xff    	
      };

    }

    begin {
      nextStatus = lastStatus;
      nextByteNum = byteNum;
    }    

    go {
      if ( in.present() ) {
	// The input is present -- advance the state
	
	int inVal = int(in.get());

	if ( isStatus(inVal) ) {

	  // A status byte

	  if ( isRealtime(inVal) ) {

	    // A System Realtime status byte
	    
	    if  ( inVal == SystemReset ) {
	      reset.emit() << int(TRUE);
	    }
	    
	  } else {

	    // Some other status byte -- remember it

	    nextStatus = inVal;
	    
	    // Reset the byte counter
	    nextByteNum = 0;

	  }

	} else {

	  // A data byte

	  int status = int(lastStatus);

	  if ( isSystem(status) ) {

	    // Last status was a system common

	  } else {

	    // Last status was a voice command

	    int byte = int(byteNum);

	    switch ( status & 0xf0 ) {
	    case NoteOff:

	      if ( byte == 0 ) {
		// Remember the pitch value
		lastPitch = inVal;
		nextByteNum = 1;
	      } else {
		// This byte is the velocity -- emit everything
		offPitch.emit() << int(lastPitch);
		offVelocity.emit() << inVal;
		offChannel.emit() << int(status & 0xf);
		nextByteNum = 0;
	      }

	      break;

	    case NoteOn:

	      if ( byte == 0 ) {
		// Remember the pitch value
		lastPitch = inVal;
		nextByteNum = 1;
	      } else {
		// This byte is the velocity -- emit everything
		if ( inVal == 0 ) {
		  // Velocity is zero -- emit a NoteOff command
		  offPitch.emit() << int(lastPitch);
		  offVelocity.emit() << 64;
		  offChannel.emit() << int(status & 0xf);
		} else {
		  // Velocity is non-zero -- emit a NoteOn command
		  onPitch.emit() << int(lastPitch);
		  onVelocity.emit() << inVal;
		  onChannel.emit() << int(status & 0xf);
		}
		nextByteNum = 0;
	      }

	      break;

	    case ChannelPitchwheel:

	      if ( byte == 0 ) {
		// Remember the least significant seven bits
		lastPitch = inVal;
		nextByteNum = 1;
	      } else {
		// This byte is the most significant -- emit everything
		pitchBend.emit() << ((int(inVal) << 7) | lastPitch);
		pitchChannel.emit() << int(status & 0xf);
		nextByteNum = 0;
	      }

	      break;

	    default:
	      break;
	    }
	  }	  
	}
      }

      if ( !onPitch.known() )		{ onPitch.makeAbsent(); }
      if ( !onVelocity.known() )	{ onVelocity.makeAbsent(); }
      if ( !onChannel.known() )		{ onChannel.makeAbsent(); }
      if ( !offPitch.known() )		{ offPitch.makeAbsent(); }
      if ( !offVelocity.known() )	{ offVelocity.makeAbsent(); }
      if ( !offChannel.known() )	{ offChannel.makeAbsent(); }
      if ( !pitchBend.known() )		{ pitchBend.makeAbsent(); }
      if ( !pitchChannel.known() )	{ pitchChannel.makeAbsent(); }
      if ( !reset.known() )		{ reset.makeAbsent(); }

    }

    tick {
	lastStatus = nextStatus;
	byteNum = nextByteNum;
    }

}

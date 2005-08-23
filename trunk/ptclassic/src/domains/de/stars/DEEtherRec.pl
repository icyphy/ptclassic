defstar {
	name { EtherRec }
	domain { DE }
	derivedfrom { Ether }
	version { @(#)DEEtherRec.pl	1.11	01/27/97 }
	author { Edward A. Lee  and Tom M. Parks }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
This star receives floating-point particles transmitted to it by
an EtherSend star.  The particle is produced at the output after
some duration of transmission specified at the transmitter.
	}
	htmldoc {
This star is derived from
<tt>Ether.</tt>
Please see the manual entry for that star for a basic introduction.
<p>
This star implements a receiver for a shared medium when the received
particle is a simple floating point number.
A similar star could be made for other
<tt>Particle</tt>
types, so this should be viewed as a concept demonstration only.
<p>
The star will produce an output only if some transmitter
(which must be also derived from
<tt>EtherSend )</tt>
uses the same medium and transmits to this receivers address.
The time at which the output is produced is specified by the transmitter
when it transfers the particle by calling the public
method
<tt>schedReception</tt>
defined in this star.
That method takes two arguments, a
<tt>Particle</tt>
reference, and a
<tt>double</tt>
giving the time at which the particle should be sent to the output.
	}
	output {
	    name { recData }
	    type { float }
	    desc { Received data. }
	}
	state {
	    name { recName }
	    type { string }
	    default { "0" }
	    desc { Unique name of the receiver. }
	}
	private {
	    // Store a received particle.
	    Particle* recdData;

	    // Use a local private variable to ensure that received
	    // data is output exactly once.
	    int dataValid;

	    // Flag used to identify the first firing
	    int firstFiring;
	}
	constructor {
	    recdData = 0;
	    dataValid = FALSE;
	    firstFiring = TRUE;
	}
	method {
	    name { schedReception }
	    access { public }
	    arglist { "(Particle& data, double firingTime)" }
	    type { void }
	    code {
		if (dataValid)
		    Error::warn(*this,"Overwriting previously received data");
		recdData = data.clone();
		refireAtTime(firingTime);
		// We have to force the refire event out to the global queue
		// by hand; normally DEStar::run handles this, but we have
		// not been called from DEStar::run.
		feedbackOut->sendData();
		dataValid = TRUE;
	    }
	}
	begin {
	    DEEther::begin();

	    if(*((const char *) recName) == '-') {
		Error::warn(*this,
			    "Receiver address should not start with '-'");
	    }

	    if (!registerReceiver(recName, this)) {
		Error::warn(*this,
	"Additional receivers with the same name will be ignored. Name is ",
                        recName);
	    }

	    // Initialize dataValid flag to indicate that we don't now have
	    // valid data pending output.
	    recdData = 0;
	    dataValid = FALSE;
	    firstFiring = TRUE;
	}
	go {
	    if (dataValid) {
		// Output the stored data
		recData.put(arrivalTime) = *recdData;

		// Release the particle
		recdData->die();

		// Mark that we no longer have valid data waiting for output
		dataValid = 0;
	    }
	}
}

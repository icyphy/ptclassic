defstar {
	name {EtherSend}
	domain {DE}
	derivedfrom { DEEther }
	version { $Id$ }
	author { E. A. Lee and T. Parks }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
This star can transmit particles of any type to any or all receivers
that have the same value for the "medium" parameter.  The receiver
address is given by the "address" input, and it must be an integer.
If the integer is negative, then copies of the particle are sent
to all receivers that use the same medium.

The transmitter "occupies" the medium for the specified duration.
A collision occurs if the medium is occupied when a transmission is
requested.  In this case,
the data to be transmitted is sent to the "collision" output.
	}
	explanation {
This star is derived from
.c Ether.
Please see the manual entry for that star for a basic introduction.
.pp
This star implements a transmitter for a shared medium.
Any type of
.c Particle
can be transmitted.
If the "address" input is a non-negative integer, then the transmitter
looks for a receiver that shares the medium that has a matching address.
If it is found, a copy of the Particle is sent to the receiver.
The receiver is told to output that particle after a certain
elapsed time, given by the most recent value that arrived on
the "duration" input.
If no such particle has arrived, a duration of zero is used.
If the address is a negative integer, then the data particle
is broadcast to all receivers on the medium.
	}
	input {
	    name { address }
	    type { int }
	    desc { Address of the receiver. Negative for broadcast. }
	}
	input {
	    name { sendData }
	    type { anytype }
	    desc { Data to transmit. }
	}
	input {
	    name { duration }
	    type { float }
	    desc { Time delay to receiver, and time occupying the medium. }
	}
	output {
	    name { collision }
	    type { = sendData }
	    desc { Indicate the medium is occupied. }
	} 
	ccinclude { "DEEtherRec.h" }
	private {
	    double latestDuration;

	    // The address is stored as a String.
	    StringList latestAddress;

	    // flag determining that the next particle(s) should be broadcast
	    int broadcast;
	}
	begin {
	    DEEther::begin();
	    broadcast = 0;
	    latestDuration = 0.0;
	}
	go {
	    if (duration.dataNew) {
		latestDuration = double(duration.get());
	    }
	    if (address.dataNew) {
		Particle& adp = address.get();
		if (int(adp) < 0) broadcast = 1;
		else broadcast = 0;
		latestAddress = (address.get()).print();
	    }
	    if (sendData.dataNew) {
		Particle& pp = sendData.get();
		if (med->occupiedAt(arrivalTime)) {
		    collision.put(arrivalTime) = pp;
		} else {
		    if (!broadcast) {
			DEEtherRec* receiveStar =
				med->findReceiver(latestAddress);
			if (receiveStar == 0) {
			    Error::warn(*this,
			    "Attempt to transmit to non-existant receiver ",
			    latestAddress);
			    return;
			}
			receiveStar->schedReception(pp,
				arrivalTime + latestDuration);
		    } else {
		        // handle broadcast case
			HashTableIter stars(med->receivers);
			HashEntry* starEntry;
			while ((starEntry = stars++) != 0) {
			    DEEtherRec* receiveStar =
				(DEEtherRec*)(starEntry->value());
			    receiveStar->schedReception(pp,
				arrivalTime + latestDuration);
			}
		    }
		    med->occupyUntil(arrivalTime + latestDuration);
		}
	    }
	}
}

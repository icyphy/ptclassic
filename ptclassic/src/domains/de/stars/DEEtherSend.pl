defstar {
	name { EtherSend }
	domain { DE }
	derivedfrom { Ether }
	version { $Id$ }
	author { Edward A. Lee and Tom M. Parks }
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
address string is given by the "address" input.  If something other than
a string is passed, Particle::print() is used to convert it to a string.
If the address string begins with '-', then copies of the data particle
are sent to all receivers that use the same medium.

The transmitter "occupies" the medium for the specified duration.
A collision occurs if the medium is occupied when a transmission is
requested.  In this case,
the data to be transmitted is sent to the "collision" output.
	}
	htmldoc {
This star is derived from
<tt>Ether.</tt>
Please see the manual entry for that star for a basic introduction.
<p>
This star implements a transmitter for a shared medium.
Any type of
<tt>Particle</tt>
can be transmitted.
If the "address" string does not start with '-', then the transmitter
looks for a receiver that shares the medium that has a matching address.
If it is found, a copy of the Particle is sent to the receiver.
The receiver is told to output that particle after a certain
elapsed time, given by the most recent value that arrived on
the "duration" input.
If no such particle has arrived, a duration of zero is used.
If the address does start with '-', then the data particle
is broadcast to all receivers on the medium.
	}
	input {
	    name { address }
	    type { anytype }
	    desc { Address of the receiver. }
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
	    latestDuration = 0.0;
	    latestAddress = "";
	    broadcast = FALSE;
	}
	go {
	    if (duration.dataNew) {
		latestDuration = double(duration.get());
	    }
	    if (address.dataNew) {
		latestAddress = (address.get()).print();
		if (*((const char *) latestAddress) == '-') broadcast = 1;
		else broadcast = 0;
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
			    "Attempt to transmit to non-existent receiver ",
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

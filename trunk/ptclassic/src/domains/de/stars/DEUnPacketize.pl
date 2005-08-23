defstar {
	name { UnPacketize }
	domain { DE }
	version { @(#)DEUnPacketize.pl	2.10 3/2/95 }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Convert a stream of packets of type FloatVecData into floating output data.
The "data" input feeds packets to the star.  Whenever a packet arrives, the
previous packet, if any, is discarded; any remaining contents are
discarded.  The "demand" input requests output data.  If there is no
data left in the current packet, the last output datum is repeated
(zero is used if there has never been a packet).  Otherwise the next
data value from the current input packet is output.
	}
	hinclude { "FloatVecData.h" }
	input {
		name { data }
		type { message }
	}
	input {
		name { demand }
		type { anytype }
	}
	output {
		name { output }
		type { float }
	}
	protected {
		int idx;
		Envelope env;
		const FloatVecData* currentMessage;
		float lastOutput;
	}
	constructor {
		data.triggers();
		data.before(demand);
	}
	setup {
		idx = 0;
		currentMessage = 0;
		lastOutput = 0;
	}
	go {
		if (data.dataNew) {
			// a new message has arrived
			data.get().getMessage(env);
			idx = 0;
			if (!env.typeCheck("FloatVecData")) {
				Error::abortRun(*this,
					env.typeError("FloatVecData"));
				return;
			}
			currentMessage = (const FloatVecData*)env.myData();
		}
		if (demand.dataNew) {
			// need to write new output
			demand.dataNew = FALSE;
			if (currentMessage && idx < currentMessage->length())
				lastOutput = currentMessage->data[idx++];
			output.put(arrivalTime) << lastOutput;	
		}
	}
}

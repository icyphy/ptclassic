defstar {
	name { IntUnPacketize }
	domain { DE }
	version { $Id$ }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Convert a stream of packets into integer output data.  The "data"
input feeds packets to the star.  Whenever a packet arrives, the
previous packet, if any, is discarded; any remaining contents are
thrown away.  The "demand" input requests output data.  If there is no
data left in the current packet, the last output datum is repeated
(zero is used if there has never been a packet).  Otherwise the next
data value from the current input packet is output.
	}
	hinclude { "IntVecData.h" }
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
		type { int }
	}
	protected {
		int idx;
		Envelope env;
		const IntVecData* currentMessage;
		int lastOutput;
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
			TYPE_CHECK(env,"IntVecData");
			currentMessage = (const IntVecData*)env.myData();
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

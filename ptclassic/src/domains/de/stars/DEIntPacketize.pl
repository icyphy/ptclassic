defstar {
	name { IntPacketize }
	domain { DE }
	version { $Id$ }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Convert integer input data into a stream of packets.  A packet is produced
when either an input appears on the demand input or when "maxLength" data
values have arrived.  Note that this star produces a null packet
if it receives a demand signal and there is no data.
	}
	ccinclude { "IntVecData.h" }
	input {
		name { data }
		type { int }
	}
	input {
		name { demand }
		type { anytype }
	}
	output {
		name { output }
		type { message }
	}
	defstate {
		name { maxLength }
		type { int }
		default { 50 }
		desc { Maximum packet length. }
	}
	protected {
		int *saveData;
		int idx;
	}
	constructor {
		saveData = 0;
		data.triggers();
		data.before(demand);
	}
	setup {
		LOG_DEL; delete [] saveData;
		LOG_NEW; saveData = new int[int(maxLength)];
		idx = 0;
	}
	go {
		if (data.dataNew)
			saveData[idx++] = data.get();
		if (demand.dataNew || idx == int(maxLength)) {
			demand.dataNew = FALSE;
			// create a IntVecData and embed it in an Envelope.
			LOG_NEW; IntVecData *p = new IntVecData(idx, saveData);
			Envelope env(*p);
			// copy the Envelope to the output
			output.put(arrivalTime) << env;
			idx = 0;
		}
	}
	destructor {
		LOG_DEL; delete [] saveData;
	}
}

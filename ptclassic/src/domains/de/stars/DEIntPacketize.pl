defstar {
	name { IntPacketize }
	domain { DE }
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1992 The Regents of the University of California }
	location { DE main library }
	desc {
Convert integer input data into a stream of packets.  A packet is produced
when either an input appears on the demand input or when "maxLength" data
values have arrived.  Note that a null packet is produced if a demand
signal arrives and there is no data.
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
		type { packet }
	}
	defstate {
		name { maxLength }
		type { int }
		default { 50 }
		desc { Maximum packet length }
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
	start {
		LOG_DEL; delete saveData;
		LOG_NEW; saveData = new int[int(maxLength)];
		idx = 0;
	}
	go {
		if (data.dataNew)
			saveData[idx++] = data.get();
		if (demand.dataNew || idx == int(maxLength)) {
			demand.dataNew = FALSE;
			// create a IntVecData and imbed it in a Packet
			LOG_NEW; IntVecData *p = new IntVecData(idx,saveData);
			Packet pkt(*p);
			// copy the Packet to the output
			output.put(arrivalTime) << pkt;
			idx = 0;
		}
	}
	destructor {
		LOG_DEL; delete saveData;
	}
}

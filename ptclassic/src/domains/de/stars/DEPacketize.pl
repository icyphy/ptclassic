defstar {
	name { Packetize }
	domain { DE }
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
Convert floating input data into a stream of packets.  A packet is produced
when either an input appears on the demand input or when "maxLength" data
values have arrived.  Note that a null packet is produced if a demand
signal arrives and there is no data.
	}
	ccinclude { "FloatVecData.h" }
	input {
		name { data }
		type { float }
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
		float *saveData;
		int idx;
	}
	constructor {
		saveData = 0;
		data.triggers();
		data.before(demand);
	}
	start {
		delete saveData;
		saveData = new float[int(maxLength)];
		idx = 0;
	}
	go {
		if (data.dataNew)
			saveData[idx++] = data.get();
		if (demand.dataNew || idx == int(maxLength)) {
			demand.dataNew = FALSE;
			// create a FloatVecData and imbed it in a Packet
			FloatVecData *p = new FloatVecData(idx,saveData);
			Packet pkt(*p);
			// copy the Packet to the output
			output.put(arrivalTime) << pkt;
			idx = 0;
		}
	}
	destructor {
		delete saveData;
	}
}

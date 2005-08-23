defstar {
	name { Packetize }
	domain { DE }
	version { @(#)DEPacketize.pl	2.11 3/2/95 }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc {
Convert floating-point input data into a stream of packets of type FloatVecData.
A packet is produced when either an input appears on the demand input or when
"maxLength" data values have arrived.  Note that a null packet is produced if
a demand signal arrives and there is no data.
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
		type { message }
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
	setup {
		LOG_DEL; delete [] saveData;
		LOG_NEW; saveData = new float[int(maxLength)];
		idx = 0;
	}
	go {
		if (data.dataNew)
			saveData[idx++] = data.get();
		if (demand.dataNew || idx == int(maxLength)) {
			demand.dataNew = FALSE;
			// create a FloatVecData and imbed it in an Envelope
			LOG_NEW; FloatVecData *p = new FloatVecData(idx,saveData);
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

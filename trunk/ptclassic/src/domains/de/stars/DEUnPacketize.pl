defstar {
	name { UnPacketize }
	domain { DE }
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	desc {
Convert a stream of packets into floating output data.  The "data"
input feeds packets to the star.  Whenever a packet arrives, the
previous packet, if any, is discarded; any remaining contents are
thrown away.  The "demand" input requests output data.  If there is no
data left in the current packet, the last output datum is repeated
(zero is used if there has never been a packet).  Otherwise the next
data value from the current input packet is output.
	}
	hinclude { "FloatVecData.h" }
	input {
		name { data }
		type { packet }
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
		Packet pkt;
		const FloatVecData* currentPacket;
		float lastOutput;
	}
	constructor {
		data.triggers();
		data.before(demand);
	}
	start {
		idx = 0;
		currentPacket = 0;
		lastOutput = 0;
	}
	go {
		if (data.dataNew) {
			// a new packet has arrived
			data.get().getPacket(pkt);
			idx = 0;
			if (!pkt.typeCheck("FloatVecData")) {
				Error::abortRun(*this,
					pkt.typeError("FloatVecData"));
				return;
			}
			currentPacket = (const FloatVecData*)pkt.myData();
		}
		if (demand.dataNew) {
			// need to write new output
			demand.dataNew = FALSE;
			if (currentPacket && idx < currentPacket->length())
				lastOutput = currentPacket->data[idx++];
			output.put(arrivalTime) << lastOutput;	
		}
	}
}

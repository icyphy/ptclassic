defstar {
	name { CellLoad }
	domain { DE }
	author { GSWalter }
	version { $Id$ }
	copyright { 1992 (c) U. C. Regents }
	location { DE main palette }

	desc {
Reads in an Envelope, extracts its Message, and
outputs that Message in a NetworkCell.
	}

	input {
		name { input }
		type { message }
	}
	output {
		name { output }
		type { message }
	}
	defstate {
		name { mssgSize }
		type { int }
		default { 384 }
		desc { size of message loaded into NetworkCell }
	}
	defstate {
		name { destination }
		type { int }
		default { 1 }
		desc { destination address }
	}
	defstate {
		name { priority }
		type { int }
		default { 1 }
		desc { 1: high, 0: low }
	}

	hinclude { "NetworkCell.h" }

	go {
		if (input.dataNew) {
			completionTime = arrivalTime;
			Envelope inEnvlp;
			input.get().getMessage(inEnvlp);
			Message* toLoad = inEnvlp.writableCopy();
			LOG_NEW; NetworkCell* shipItOut = new NetworkCell(
					*toLoad, int( priority ), int( destination ),
					int( mssgSize ), 0,
					float( completionTime), 0 );
			Envelope outEnvlp( *shipItOut );
			output.put( completionTime ) << outEnvlp;
		} // end if
	} // end go
} // end defstar

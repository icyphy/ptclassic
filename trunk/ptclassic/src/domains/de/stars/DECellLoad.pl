defstar {
	name { CellLoad }
	domain { DE }
	author { GSWalter }
	version { @(#)DECellLoad.pl	1.8 10/23/95 }
        copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { DE main library }

	desc {
Read in an Envelope, extract its Message, and
output that Message in a NetworkCell.
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

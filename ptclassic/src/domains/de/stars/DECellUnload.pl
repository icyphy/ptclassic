defstar {
	name { CellUnload }
	domain { DE }
	author { GSWalter }
	version { $Id$ }
WARNING: CHECK COPYRIGHT - not UC
	copyright { 1992 (c) U. C. Regents }
	location { DE main palette }
	desc {
Removes Message from NetworkCell in DE domain.
	}

	input { name { input } type { message } }
	output { name { output } type { message } }

	hinclude { "NetworkCell.h" }

	go {
		if ( input.dataNew ) {
			Envelope inEnvlp;
			input.get().getMessage( inEnvlp );
			TYPE_CHECK( inEnvlp, "NetworkCell" );

// Need "writableCopy()" and "writableData()" below because the Envelope
// constructor below cannot take a const Message as input.
			NetworkCell* cellPtr = (NetworkCell*)
					inEnvlp.writableCopy();
			Message* outMssg = cellPtr->writableData();
			Envelope outEnvlp(*outMssg);
			output.put(arrivalTime) << outEnvlp;
		} // end if
	} // end go
} // end defstar
